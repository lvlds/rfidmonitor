#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <stdlib.h>

#include "tcpserver.h"
#include "../Client/datapacket.h"

TCPServer::TCPServer(QObject *parent) :
    QObject(parent)
{
    m_server = new QTcpServer(this);
}

void TCPServer::start()
{
    if(m_server->listen(QHostAddress::Any, 9999)){
        connect(m_server, SIGNAL(newConnection()), SLOT(newConnection()));
    }
    QTimer::singleShot(1000, this, SLOT(broadcast()));
}

void TCPServer::newConnection()
{
    QTcpSocket *newConn = m_server->nextPendingConnection();
    if(newConn){

        connect(newConn, SIGNAL(disconnected()), SLOT(onDisconnected()));
        connect(newConn, SIGNAL(readyRead()), SLOT(readyRead()));
    }
}

void TCPServer::broadcast()
{
    QList<QTcpSocket *> connections = findChildren<QTcpSocket *>();

    /*
        KRequest = 0x55000000,
        KSet = 0xAA000000,
        KData = 0x4,
        KTemperature = 0x8,
        KDateTime = 0x10,
        KIPAddress = 0x20,
        KName = 0x40,
        KMAC = 0x80,
        KID = 0x100,
        KTimeOuts = 0x200,
        KSynchronizeAll = 0x400
    */
    // 9 items
    quint32 messageType[] = {KRequest, KSet};
    quint32 requestFlags[] = {0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400};
    quint32 setFlags[] = {0x10, 0x40, 0x100, 0x200};

    int index = rand() % 2;

    if((messageType[index] & KRequest) == KRequest){
        int index = rand() % 9;

        ServerPacketHeader packet;
        packet.flags = (requestFlags[index] | KRequest);
        packet.dateTime = get_date(QDateTime::currentDateTime());

        foreach (QTcpSocket *socket, connections) {
            socket->write((char *)&packet, sizeof(ServerPacketHeader));
        }

        //        QTimer::singleShot(10000, this, SLOT(broadcast()));
    }else{
        int index = rand() % 4;

        ServerPacketHeader packet;
        packet.flags = (setFlags[index] | KSet);
        packet.dateTime = get_date(QDateTime::currentDateTime());

        foreach (QTcpSocket *socket, connections) {
            socket->write((char *)&packet, sizeof(ServerPacketHeader));
        }
    }
    QTimer::singleShot(1000, this, SLOT(broadcast()));
}

void TCPServer::onDisconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket *>(sender());
    socket->deleteLater();
}

void TCPServer::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket *>(sender());
    if(socket){
        QByteArray received = socket->readAll();

        char buffer[sizeof(ClientPacketHeader)];
        memcpy(buffer, received.data(), sizeof(ClientPacketHeader));
        ClientPacketHeader *clientHeader = reinterpret_cast<ClientPacketHeader *>(buffer);
        if(clientHeader) {
            /*
                KRequest = 0x55000000,
                KSet = 0xAA000000,
                KData = 0x4,
                KTemperature = 0x8,
                KDateTime = 0x10,
                KIPAddress = 0x20,
                KName = 0x40,
                KMAC = 0x80,
                KID = 0x100,
                KTimeOuts = 0x200,
                KSynchronizeAll = 0x400
            */
            if((clientHeader->flags & KRequest) == KRequest){
                // The server sent a message that needs to be answered with some information
                int option = clientHeader->flags &~ KRequest;
                switch (option) {
                case KDateTime:
                    // Update time of the Reading Point
                    break;
                case KData:
                case KIPAddress:
                case KSynchronizeAll:
                case KTimeOuts:
                case KID:
                case KMAC:
                case KName:
                case KTemperature:
                    break;
                default:
                    qDebug() << "KRequest: Unknown request. - " << Q_FUNC_INFO;
                    break;
                }
            }else if ((clientHeader->flags & KSet) == KSet){
                // The server sent a message that contains settings for the Collection Point
                int option = clientHeader->flags &~ KSet;
                switch (option) {
                case KData:
                {

                    int dataSize = clientHeader->MessageUnion.dataSize*sizeof(RfidData);
                    if(received.size() == (int)(dataSize + sizeof(ClientPacketHeader))){
                        char dataReceived[dataSize];
                        memcpy(dataReceived, &(received.data()[sizeof(ClientPacketHeader)]), dataSize);
                        for(quint32 i=0; i < clientHeader->MessageUnion.dataSize; i++){
                            RfidData rfid;
                            memcpy(&rfid, &(dataReceived[i*sizeof(RfidData)]), sizeof(RfidData));
                            //                        qDebug() << QString("id = %1, appcode = %2, idcode = %3, datetime = %4").arg(rfid.id).arg(rfid.applicationcode).arg(rfid.identificationcode).arg(get_date(rfid.dateTime).toString());
                        }
                        qDebug() << QString("KData\nId Sender = %1\n"
                                            "Name = %2\n"
                                            "Temperature = %3\n"
                                            "Date Time = %4\n"
                                            "MAC Address = %5\n"
                                            "Packet Size = %6").arg(clientHeader->idsender).
                                    arg(clientHeader->name).
                                    arg(clientHeader->temperature).
                                    arg(get_date(clientHeader->dateTime).toString()).
                                    arg(clientHeader->MessageUnion.MAC_Address.macAddress).
                                    arg(sizeof(ClientPacketHeader)+dataSize);
                    }
                    break;
                }
                case KIPAddress:
                    qDebug() << QString("KIPAddress\nReceived Ip Address: %1 from Client: %2").arg(clientHeader->MessageUnion.IP_Address.ipAddress).arg(clientHeader->name);
                    break;
                case KMAC:
                    qDebug() << QString("KMAC\nReceived MAC Address: %1 from Client: %2").arg(clientHeader->MessageUnion.MAC_Address.macAddress).arg(clientHeader->name);
                    break;
                case KTemperature:
                    qDebug() << QString("KTemperature\nReceived Temperature: %1 from Client: %2").arg(clientHeader->temperature).arg(clientHeader->name);
                    break;
                case KDateTime:
                    qDebug() << QString("KDateTime\nReceived Date & Time: %1 from Client: %2").arg(get_date(clientHeader->dateTime).toString()).arg(clientHeader->name);
                    break;
                case KName:
                    qDebug() << QString("KName\nId Sender = %1\n"
                                        "Name = %2\n"
                                        "Temperature = %3\n"
                                        "Date Time = %4\n"
                                        "MAC Address = %5\n").arg(clientHeader->idsender).
                                arg(clientHeader->name).
                                arg(clientHeader->temperature).
                                arg(get_date(clientHeader->dateTime).toString()).
                                arg(clientHeader->MessageUnion.TimeOuts.readerTimeOut);
                    break;
                case KID:
                    qDebug() << QString("KID\nId Sender = %1\n"
                                        "Name = %2\n"
                                        "Temperature = %3\n"
                                        "Date Time = %4\n"
                                        "MAC Address = %5\n").arg(clientHeader->idsender).
                                arg(clientHeader->name).
                                arg(clientHeader->temperature).
                                arg(get_date(clientHeader->dateTime).toString()).
                                arg(clientHeader->MessageUnion.TimeOuts.readerTimeOut);
                    break;
                case KTimeOuts:
                    qDebug() << QString("KTimeOuts\nId Sender = %1\n"
                                        "Name = %2\n"
                                        "Temperature = %3\n"
                                        "Date Time = %4\n"
                                        "Reader Timeout = %5\n"
                                        "Synchronization Timeout = %6").arg(clientHeader->idsender).
                                arg(clientHeader->name).
                                arg(clientHeader->temperature).
                                arg(get_date(clientHeader->dateTime).toString()).
                                arg(clientHeader->MessageUnion.TimeOuts.readerTimeOut).
                                arg(clientHeader->MessageUnion.TimeOuts.synTimeOut);
                    break;
                case KSynchronizeAll:
                    break;
                case KGeneralInfo:
                {
                    qDebug() << QString("KGeneralInfo\nId Sender = %1\n"
                                        "Name = %2\n"
                                        "Temperature = %3\n"
                                        "Date Time = %4\n"
                                        "MAC Address = %5").arg(clientHeader->idsender).
                                arg(clientHeader->name).
                                arg(clientHeader->temperature).
                                arg(get_date(clientHeader->dateTime).toString()).
                                arg(clientHeader->MessageUnion.MAC_Address.macAddress);

                    break;
                }
                default:
                    qDebug() << "KSet: Unknown request. - " << Q_FUNC_INFO;
                    break;
                }
            }else{
                // The validation bits are invalid
                qDebug() << "The Validation bits are corrupted. - " << Q_FUNC_INFO;
            }
        }
    }
}
