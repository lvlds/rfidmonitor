#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QDateTime>

class QTcpSocket;

struct Client
{
    quint32 id;
    char name[24];
    QDateTime dateTime;
    float temperature;
};

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = 0);

signals:

public slots:
    void start();

    void readyRead();
    void sendRandomData();
    void sendRandomDataNewDataPacket();
    void sendRandomClientPacket();

    void connected();
    void disconnected();

    void handleStateChanged(QAbstractSocket::SocketState socketState);
    void handleError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;

    /*!
     * \brief syncData creates a ClientHeader and sets the next values
     *  - flags (KSet = 0xAA000000, KData = 0x4)
     *  - idSender
     *  - name
     *  - temperature
     *  - datetime
     *  - MessageUnion.dataSize
     *  - MessageBlock
     */
    void syncData();
    void syncTemperature();
    void syncDateTime();
    void syncIPAddress();
    void syncName();
    void syncMAC();
    void syncID();
    void syncTimeouts();
    void syncAll();

    /*!
     * \brief syncGeneralInfo General Info Synchronization will send the next information:
     * - flags
     * - idSender
     * - name
     * - temperature
     * - datetime
     * - MessageUnion -> MAC Address
     */
    void syncGeneralInfo();

    // Only for testing purposes
    Client m_testClient;
    void createTestClient();
};

#endif // TCPCLIENT_H
