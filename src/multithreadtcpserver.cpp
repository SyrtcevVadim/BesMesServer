#include<thread>
#include<QTcpSocket>
#include "multithreadtcpserver.h"

size_t MultithreadTcpServer::workerThreadsNumber = std::thread::hardware_concurrency();

MultithreadTcpServer::MultithreadTcpServer(QHostAddress serverIPAddress,
                                           qint16 serverPort,
                                           QObject *parent):
    QTcpServer(parent), serverIPAddress(serverIPAddress), serverPort(serverPort)
{
   initWorkers();
   configureStatisticsCounter();
}

MultithreadTcpServer::~MultithreadTcpServer()
{
    removeWorkers();
    delete statisticsCounter;
}

void MultithreadTcpServer::configureStatisticsCounter()
{
    statisticsCounter = new ServerStatisticsCounter(this);
    // Объект счётчик узнаёт об открытии нового клиентского соединения
    connect(this, SIGNAL(clientConnectionOpenned()),
            statisticsCounter, SLOT(increaseActiveConnectionsCounter()));
    connect(this, SIGNAL(clientConnectionOpenned()),
            statisticsCounter, SLOT(increaseTotalEstablishedConnectionsCounter()));
    // Объект счётчик узнаёт о закрытии клиентского соединения
    connect(this, SIGNAL(clientConnectionClosed()),
            statisticsCounter, SLOT(decreaseActiveConnectionsCounter()));
    // Пробрасываем сигнал об изменении количества активных соединений "во вне"
    connect(statisticsCounter, SIGNAL(activeConnectionsCounterChanged(unsigned long long)),
            SIGNAL(activeConnectionsCounterChanged(unsigned long long)));
}

void MultithreadTcpServer::start()
{
    // Начинаем слушать входящие соединения
    listen(serverIPAddress, serverPort);
    qDebug() << "Сервер прослушивает входящие соединения!";
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
    // Отправляем сигнал о том, что нужно остановить рабочие потоки (отключаем все соединения от них)
    emit serverStopped();
    qDebug() << "Сервер не прослушивает входящие соединения";
}

void MultithreadTcpServer::removeWorkers()
{
    for(ServerWorker *workerForDeletion: serverWorkers)
    {
        workerForDeletion->quit();
    }
}

void MultithreadTcpServer::initWorkers()
{
    /* Значение possibleThreadNumber может оказаться равным нулю.
     * Это может произойти, если это значение изначально неопределено или не поддаётся расчёту.
     *
     * Функция std::thread::hardware_concurrency() даёт лишь рекомендацию
     * об оптимальном количестве потоков. Тем не менее мы будем ей следовать
     */
    if(workerThreadsNumber == 0)
    {
        workerThreadsNumber = DEFAULT_THREAD_NUMBER;
    }
    // Создаём потоки обработки входящих соединений
    for(int i{0}; i < workerThreadsNumber; i++)
    {
        ServerWorker *newWorker = new ServerWorker(this);
        /* Когда работа сервера останавливается, рабочим потокам отправляется сигнал
         * Мы отправляем именно сигнал, а не слот, поскольку рабочий поток не хранит объекты подключений в коллекции
         * Объекты подключения, получив данный сигнал, разрывают своё соединение с сервером. Таким образом, нагрузка на рабочий поток
         * останавливается. Сервер после этого можно считать простаивающим
         */
        connect(this, SIGNAL(serverStopped()), newWorker, SIGNAL(stopped()));
        // Пробрасываем сигнал о разрыве клиентского соединения "во вне"
        connect(newWorker, SIGNAL(clientConnectionClosed()), SIGNAL(clientConnectionClosed()));
        serverWorkers.append(newWorker);
    }
}



void MultithreadTcpServer::incomingConnection(qintptr socketDescriptor)
{
    /*
     * Значение общего числа подключений к серверу за текущий сеанс используется для балансировки нагрузки
     * от новых подключений между рабочими потоками сервера по алгоритму Round Robin
     */
    unsigned long long totalEstablishedConnectionsCounter = statisticsCounter->getTotalEstablishedConnectionsCounter();
    qDebug() << "Общее количество подключений за сеанс: " << totalEstablishedConnectionsCounter;
    qDebug() << "Направляем новое подключение потоку "<< totalEstablishedConnectionsCounter%workerThreadsNumber;
    // Выбираем серверный рабочий поток, который будет ответственен за обработку сообщений от нового подключения
    ServerWorker *currentWorker = serverWorkers[totalEstablishedConnectionsCounter%workerThreadsNumber];
    currentWorker->addClientConnection(socketDescriptor);
    emit clientConnectionOpenned();
}

