#include <QCoreApplication>
#include <QDebug>

#include "datapacket.h"
#include "tcpclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TcpClient client;

    QStringList args = a.arguments();
    if(args.contains("-h") && args.size() == 3){
        client.start(args.at(2));
    }else{
        qDebug() << "Invalid arguments!";
        a.quit();
    }

    return a.exec();
}
