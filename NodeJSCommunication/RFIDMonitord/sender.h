#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QAbstractSocket>

class QTcpSocket;

class Sender : public QObject
{
    Q_OBJECT
public:
    Sender(QString host, int port, QObject *parent=0);

    void start();

private:
    QString m_hostName;
    int m_port;
    QTcpSocket *m_socket;

public slots:
    void connected();
    void disconnected();

    void readyRead();
    void randomMessage();
    void handleError(QAbstractSocket::SocketError value);
};

#endif // SENDER_H
