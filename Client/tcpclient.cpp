#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QNetworkInterface>

#include <cmath>
#include <stdlib.h>
#include <string.h>

#include "tcpclient.h"
#include "datapacket.h"

TcpClient::TcpClient(QObject *parent) :
    QObject(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(m_socket, SIGNAL(connected()), SLOT(connected()));
    connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
    connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(handleStateChanged(QAbstractSocket::SocketState)));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(handleError(QAbstractSocket::SocketError)));

//    m_testClient.dateTime = QDateTime::currentDateTime();
//    m_testClient.temperature = 31.25;
//    memcpy(m_testClient.name, "PONTO COLETA 1", 24);

    createTestClient();
}

void TcpClient::start(const QString &server)
{
    m_socket->connectToHost(server, 9999);
}

QString state_string(int socketState){
    QString state;
    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        state = "QAbstractSocket::UnconnectedState";
        break;
    case QAbstractSocket::HostLookupState:
        state = "QAbstractSocket::HostLookupState";
        break;
    case QAbstractSocket::ConnectingState:
        state = "QAbstractSocket::ConnectingState";
        break;
    case QAbstractSocket::ConnectedState:
        state = "QAbstractSocket::ConnectedState";
        break;
    case QAbstractSocket::BoundState:
        state = "QAbstractSocket::BoundState";
        break;
    case QAbstractSocket::ListeningState:
        state = "QAbstractSocket::ListeningState";
        break;
    case QAbstractSocket::ClosingState:
        state = "QAbstractSocket::ClosingState";
        break;
    default:
        break;
    }

    return state;
}

void TcpClient::sendRandomClientPacket()
{
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
    int messageType[] = {KRequest, KSet};
//    int clientFlags[] = {0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400};
    int setFlags[] = {0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200}; // 8
    int requestFlags[] = {0x10, 0x100}; // 2

    int index = rand() % 2;

    if((messageType[index] & KRequest) == KRequest){
        // the client can ask for date & time when the system is starting up
        index = rand()%2;
        switch (requestFlags[index]) {
        case KID:

            break;
        case KDateTime:

            break;
        case KData:
        case KTimeOuts:
        case KTemperature:
        case KIPAddress:
        case KName:
        case KMAC:
        case KSynchronizeAll:
            break;
        default:
            qDebug() << "Unknown request";
            break;
        }
    }else{
        index = rand() % 8;
        switch (requestFlags[index]) {
        case KData:
            //

            break;
        case KTemperature:

            break;
        case KDateTime:

            break;
        case KIPAddress:

            break;
        case KName:

            break;
        case KMAC:

            break;
        case KID:

            break;
        case KTimeOuts:

            break;
        case KSynchronizeAll:

            break;
        default:
            qDebug() << "Unknown request";
            break;
        }
    }
}

void TcpClient::connected()
{
    sendRandomClientPacket();
}

void TcpClient::disconnected()
{
    qDebug() << "Disconnected. " << Q_FUNC_INFO;
    QTimer::singleShot(10000, this, SLOT(start()));
}

void TcpClient::handleStateChanged(QAbstractSocket::SocketState socketState)
{
    QString state = state_string(socketState);
    qDebug() << QString("State %1: %2").arg(socketState).arg(state);
}

void TcpClient::handleError(QAbstractSocket::SocketError socketError)
{
    if(m_socket->state() == QAbstractSocket::UnconnectedState){
        qDebug() << "Disconnected from server, trying again in 10 seconds. " << Q_FUNC_INFO;
        qDebug() << QString("Error %1: %2").arg(socketError).arg(m_socket->errorString());
        //        QTimer::singleShot(10000, this, SLOT(start()));
    }
}

void TcpClient::syncData()
{
    qDebug() << "Send \"Data\" to Server";
    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *  - MessageBlock
        *
    */
    ClientPacketHeader header;
    quint32 cset = KSet;
    quint32 cdata = KData;
    header.flags = KSet | KData;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);


    // create random data
    QList<RfidData> randomData;

    int quantity = rand() % 900 + 125;
    qDebug() << QString("Quantity = %1").arg(quantity);

    for(int i = 0; i < quantity; i++) {
        RfidData data;
        data.id = i + 1;
        data.idpontocoleta = rand() % 10 + 1;
        data.idantena = rand() % 3 + 1;
        data.identificationcode = rand() % 1000 + 1;
        data.applicationcode = rand() % 10000000 + 250000;
        data.dateTime = get_date(QDateTime::currentDateTime());
        randomData.append(data);
    }

    header.MessageUnion.dataSize= randomData.size();

    quint64 bufferSize = sizeof(ClientPacketHeader) + randomData.size()*sizeof(RfidData);
    char dataBuffer[bufferSize];

    // save the ClientPacketHeader
    memcpy(dataBuffer, &header, sizeof(ClientPacketHeader));

    int offset = sizeof(ClientPacketHeader);
    foreach (const RfidData &d, randomData) {
        int index = randomData.indexOf(d);
        memcpy(&(dataBuffer[offset + index*sizeof(RfidData)]), (char *)&d, sizeof(RfidData));
    }

    if(m_socket){
        qint64 bytes = m_socket->write(dataBuffer, bufferSize);
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(bufferSize);
    }
}

void TcpClient::syncTemperature()
{
    qDebug() << "Send \"Temperature\" to Server";
    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *
    */
    ClientPacketHeader header;
    header.flags = KSet | KData;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);
    header.MessageUnion.dataSize = 0;

    QString macAddress("??:??:??:??:??:??");

    foreach (QNetworkInterface interfaces, QNetworkInterface::allInterfaces()) {
        if(!(interfaces.flags() & QNetworkInterface::IsLoopBack)) {
            qDebug() << QString("Hardware Address = %1").arg(interfaces.hardwareAddress());
            macAddress = interfaces.hardwareAddress();
        }
    }

    macAddress = macAddress.remove(":");
    memcpy(header.MessageUnion.MAC_Address.macAddress, macAddress.toStdString().c_str(), 6);

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&header, sizeof(ClientPacketHeader));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg((quint32)sizeof(ClientPacketHeader));
    }
}

void TcpClient::syncDateTime()
{
    qDebug() << "Send \"Date & Time\" to Server";
    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *
    */
    ClientPacketHeader header;
    header.flags = KSet | KDateTime;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);

    QString macAddress("??:??:??:??:??:??");

    foreach (QNetworkInterface interfaces, QNetworkInterface::allInterfaces()) {
        if(!(interfaces.flags() & QNetworkInterface::IsLoopBack)) {
            qDebug() << QString("Hardware Address = %1").arg(interfaces.hardwareAddress());
            macAddress = interfaces.hardwareAddress();
        }
    }

    macAddress = macAddress.remove(":");
    memcpy(header.MessageUnion.MAC_Address.macAddress, macAddress.toStdString().c_str(), 6);

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&header, sizeof(ClientPacketHeader));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(sizeof(ClientPacketHeader));
    }
}

void TcpClient::syncIPAddress()
{
    qDebug() << "Send \"IP Address\" to Server";

    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *
    */
    ClientPacketHeader header;
    header.flags = KSet | KIPAddress;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);

    foreach (QHostAddress hostAddress, QNetworkInterface::allAddresses()) {
        if(!hostAddress.isLoopback() && (hostAddress.protocol() == QAbstractSocket::IPv4Protocol)) {
            qDebug() << QString("toString = %1, toIPv4 = %2").arg(hostAddress.toString()).arg(hostAddress.toIPv4Address());
            header.MessageUnion.IP_Address.ipAddress = hostAddress.toIPv4Address();
        }
    }

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&header, sizeof(ClientPacketHeader));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(sizeof(ClientPacketHeader));
    }
}

void TcpClient::syncName()
{
    qDebug() << "Send \"Name\" to Server";
    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *
    */
    ClientPacketHeader header;
    header.flags = KSet | KName;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);


    QString macAddress("??:??:??:??:??:??");

    foreach (QNetworkInterface interfaces, QNetworkInterface::allInterfaces()) {
        if(!(interfaces.flags() & QNetworkInterface::IsLoopBack)) {
            qDebug() << QString("Hardware Address = %1").arg(interfaces.hardwareAddress());
            macAddress = interfaces.hardwareAddress();
        }
    }

    macAddress = macAddress.remove(":");
    memcpy(header.MessageUnion.MAC_Address.macAddress, macAddress.toStdString().c_str(), 6);

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&header, sizeof(ClientPacketHeader));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(sizeof(ClientPacketHeader));
    }

}

void TcpClient::syncMAC()
{
    qDebug() << "Send \"MAC\" to Server";
    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *
    */
    ClientPacketHeader header;
    header.flags = KSet | KMAC;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);


    QString macAddress("??:??:??:??:??:??");

    foreach (QNetworkInterface interfaces, QNetworkInterface::allInterfaces()) {
        if(!(interfaces.flags() & QNetworkInterface::IsLoopBack)) {
            qDebug() << QString("Hardware Address = %1").arg(interfaces.hardwareAddress());
            macAddress = interfaces.hardwareAddress();
        }
    }

    macAddress = macAddress.remove(":");
    memcpy(header.MessageUnion.MAC_Address.macAddress, macAddress.toStdString().c_str(), 6);

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&header, sizeof(ClientPacketHeader));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(sizeof(ClientPacketHeader));
    }
}

void TcpClient::syncID()
{
    qDebug() << "Send \"ID\" to Server";
    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *
    */
    ClientPacketHeader header;
    header.flags = KSet | KID;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);


    QString macAddress("??:??:??:??:??:??");

    foreach (QNetworkInterface interfaces, QNetworkInterface::allInterfaces()) {
        if(!(interfaces.flags() & QNetworkInterface::IsLoopBack)) {
            qDebug() << QString("Hardware Address = %1").arg(interfaces.hardwareAddress());
            macAddress = interfaces.hardwareAddress();
        }
    }

    macAddress = macAddress.remove(":");
    memcpy(header.MessageUnion.MAC_Address.macAddress, macAddress.toStdString().c_str(), 6);

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&header, sizeof(ClientPacketHeader));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(sizeof(ClientPacketHeader));
    }
}

void TcpClient::syncTimeouts()
{
    qDebug() << "Send \"Reading & Synchronization Timeout\" to Server";
    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *
    */
    ClientPacketHeader header;
    header.flags = KSet | KTimeOuts;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);

    header.MessageUnion.TimeOuts.readerTimeOut = 150;
    header.MessageUnion.TimeOuts.synTimeOut = 10000;

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&header, sizeof(ClientPacketHeader));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(sizeof(ClientPacketHeader));
    }
}

void TcpClient::syncAll()
{
    qDebug() << "Send \"Data, Temperature, Date & Time, IP Address, MAC, ID and Timeouts\" to Server";
}

void TcpClient::syncGeneralInfo()
{
    qDebug() << "Send \"Temperature\" to Server";
    /*
        *  - flags (KSet = 0xAA000000, KData = 0x4)
        *  - idSender
        *  - name
        *  - temperature
        *  - datetime
        *  - MessageUnion.dataSize
        *
    */
    ClientPacketHeader header;
    header.flags = KSet | KGeneralInfo;
    header.idsender = m_testClient.id;
    memcpy(header.name, m_testClient.name, 24);
    header.temperature = m_testClient.temperature;
    header.dateTime = get_date(m_testClient.dateTime);

    QString macAddress("??:??:??:??:??:??");

    foreach (QNetworkInterface interfaces, QNetworkInterface::allInterfaces()) {
        if(!(interfaces.flags() & QNetworkInterface::IsLoopBack)) {
            qDebug() << QString("Hardware Address = %1").arg(interfaces.hardwareAddress());
            macAddress = interfaces.hardwareAddress();
        }
    }

    macAddress = macAddress.remove(":");
    memcpy(header.MessageUnion.MAC_Address.macAddress, macAddress.toStdString().c_str(), 6);

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&header, sizeof(ClientPacketHeader));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(sizeof(ClientPacketHeader));
    }
}

void TcpClient::createTestClient()
{
    srand(time(NULL));
    m_testClient.id = rand() % 100;
    m_testClient.temperature = 31.48;
    m_testClient.dateTime = QDateTime::currentDateTime();
    sprintf(m_testClient.name, "Ponto de Coleta %d", m_testClient.id);
}

void TcpClient::readyRead()
{
    QByteArray received = m_socket->readAll();

    char buffer[sizeof(ServerPacketHeader)];
    memcpy(buffer, received.data(), sizeof(ServerPacketHeader));
    ServerPacketHeader *serverHeader = reinterpret_cast<ServerPacketHeader *>(buffer);
    if(serverHeader) {
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
            KGeneralInfo = 0x800
        */
        if((serverHeader->flags & KRequest) == KRequest){
            // The server sent a message that needs to be answered with some information
            int option = serverHeader->flags &~ KRequest;
            switch (option) {
            case KData:
                // schedule synchronization of data
                syncData();
                break;
            case KTemperature:
                // schedule to send temperature
                syncTemperature();
                break;
            case KDateTime:
                // Update time of the Reading Point
                syncDateTime();
                break;
            case KIPAddress:
                // Send IP Address
                syncIPAddress();
                break;
            case KName:
                // Send Name
                syncName();
                break;
            case KMAC:
                // Send MAC Address
                syncMAC();
                break;
            case KID:
                // Change ID of the Reading Point
                syncID();
                break;
            case KTimeOuts:
                // Change Timeouts for Reading and Synchronization
                syncTimeouts();
                break;
            case KSynchronizeAll:
                // Synchronize With Server

                syncAll();
                break;
            default:
                qDebug() << "Unknown request. - " << Q_FUNC_INFO;
                break;
            }
        }else if ((serverHeader->flags & KSet) == KSet){
            // The server sent a message that contains settings for the Collection Point
            int option = serverHeader->flags &~ KSet;
            switch (option) {
            case KDateTime:
                // Update time of the Reading Point
                qDebug() << "Execute Gustavo's Script to Update the time of the Collection Point";
                break;
            case KName:
                qDebug() << "Modify the name of the CP";
                break;
            case KID:
                qDebug() << "Modify the ID of the CP";
                break;
            case KTimeOuts:
                qDebug() << "Modify the TimeOuts for Reading and Synchronization";
                break;
            case KIPAddress:
            case KMAC:
            case KData:
            case KTemperature:
            case KSynchronizeAll:
                break;
            default:
                qDebug() << "Unknown request. - " << Q_FUNC_INFO;
                break;
            }
        }else{
            // The validation bits are invalid
            qDebug() << "The Validation bits are corrupted. - " << Q_FUNC_INFO;
        }
    }
}
