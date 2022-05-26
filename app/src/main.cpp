#include <QCoreApplication>
#include "project_structure_defender.h"
#include "client_connection.h"
#include "multi_thread_tcp_server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Все строки хранятся в кодировке UTF-8
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    ProjectStructureDefender::init();
    ClientConnection::initSslConfiguration();

    MultithreadTcpServer server(QHostAddress::Any);
    server.start();

    return a.exec();
}
