#include<thread>
#include "multithreadtcpserver.h"

size_t MultithreadTcpServer::optimalThreadNumber = std::thread::hardware_concurrency();

MultithreadTcpServer::MultithreadTcpServer(QObject *parent):QTcpServer(parent)
{
    initWorkers();
    for(ServerWorker *worker:serverWorkers)
    {
        worker->start();
    }
}

void MultithreadTcpServer::removeWorkers()
{
    for(ServerWorker *workerForDeletion: serverWorkers)
    {
        workerForDeletion->quit();
    }
}

MultithreadTcpServer::~MultithreadTcpServer()
{
    removeWorkers();
}

void MultithreadTcpServer::initWorkers()
{
    /* Значение possibleThreadNumber может оказаться равным нулю.
     * Это может произойти, если это значение изначально неопределено или не поддаётся расчёту.
     *
     * Функция std::thread::hardware_concurrency() даёт лишь рекомендацию
     * об оптимальном количестве потоков. Тем не менее мы будем ей следовать
     */
    if(optimalThreadNumber == 0)
    {
        optimalThreadNumber = DEFAULT_THREAD_NUMBER;
    }
    // Создаём потоки обработки входящих соединений
    for(int i{0}; i < optimalThreadNumber; i++)
    {
        ServerWorker *newWorker = new ServerWorker(this);
        serverWorkers.append(newWorker);
    }
}
