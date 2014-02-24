#include <QCoreApplication>

#include "sender.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Sender snd("localhost", 8124);
//    Sender snd("localhost", 8080);
    snd.start();

    return a.exec();
}
