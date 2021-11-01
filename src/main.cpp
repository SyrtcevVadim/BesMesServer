#include <QtWidgets>
#include "multithreadtcpserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MultithreadTcpServer server;
    server.listen(QHostAddress::Any, 1234);
    qDebug() << "Сервер начинает прослушивать входящие соединения";
    return a.exec();
}
