#include<thread>
#include "multithreadtcpserver.h"

size_t MultithreadTcpServer::possibleThreadNumber = std::thread::hardware_concurrency();

MultithreadTcpServer::MultithreadTcpServer(QWidget *parent)
{

}
