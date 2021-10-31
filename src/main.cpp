#include <QtWidgets>
#include "multithreadtcpserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MultithreadTcpServer server;
    return a.exec();
}
