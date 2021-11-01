#include<thread>
#include<QTcpSocket>
#include "multithreadtcpserver.h"

size_t MultithreadTcpServer::threadsNumber = std::thread::hardware_concurrency();

MultithreadTcpServer::MultithreadTcpServer(QHostAddress serverIPAddress,
                                           qint16 serverPort,
                                           QObject *parent):
    QTcpServer(parent), serverIPAddress(serverIPAddress), serverPort(serverPort)
{
    incomingConnectionsCounter=0;
    initWorkers();

}
void MultithreadTcpServer::start()
{
    // Начинаем слушать входящие соединения
    listen(serverIPAddress, serverPort);
    // Запускает рабочие потоки
    for(ServerWorker *worker:serverWorkers)
    {
        worker->start();
    }
}

void MultithreadTcpServer::stop()
{
    // Приостанавливаем прослушивание входящих соединений
    close();
    // Останавливаем потоки обработчики
    for(ServerWorker *worker: serverWorkers)
    {
        worker->wait();
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
    if(threadsNumber == 0)
    {
        threadsNumber = DEFAULT_THREAD_NUMBER;
    }
    // Создаём потоки обработки входящих соединений
    for(int i{0}; i < threadsNumber; i++)
    {
        ServerWorker *newWorker = new ServerWorker(this);
        serverWorkers.append(newWorker);
    }
}

void MultithreadTcpServer::incomingConnection(qintptr socketDescriptor)
{
    // Выбираем серверного рабочего, который будет ответственен за обработку сообщений от данного подключения
    ServerWorker *currentWorker = serverWorkers[incomingConnectionsCounter%threadsNumber];
    currentWorker->addClientConnection(socketDescriptor);
    incomingConnectionsCounter++;
}
