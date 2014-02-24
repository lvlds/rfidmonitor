#include <QTcpSocket>
#include <QTimer>
#include "sender.h"

Sender::Sender(QString host, int port, QObject *parent) :
    QObject(parent),
    m_hostName(host),
    m_port(port)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()), SLOT(connected()));
    connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
    connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
}

void Sender::start()
{
    m_socket->connectToHost(m_hostName, m_port);
}

void Sender::connected()
{
    QTimer::singleShot(1000, this, SLOT(randomMessage()));
}

void Sender::disconnected()
{
    qDebug() << "Disconected";
}

void Sender::readyRead()
{
    QByteArray received = m_socket->readAll();

    QString data(received);

    qDebug() << QString("Received: %1").arg(data);
}

void Sender::randomMessage()
{
    static int count = 0;
    m_socket->write(QString("%1 - {sender: \"raspi\"}{message: \"Hello Server!\"}").arg(count).toLatin1());
    count++;
    QTimer::singleShot(1000, this, SLOT(randomMessage()));
}

void Sender::handleError(QAbstractSocket::SocketError value)
{
    qDebug() << QString("Error %1: %2").arg(value).arg(m_socket->errorString());
}
