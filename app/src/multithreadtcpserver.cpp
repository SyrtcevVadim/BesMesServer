#include<QThread>
#include<QTcpSocket>
#include "multithreadtcpserver.h"
#include "clientconnection.h"


int MultithreadTcpServer::workerThreadsNumber = QThread::idealThreadCount();

MultithreadTcpServer::MultithreadTcpServer(QHostAddress serverIPAddress,
                                           QObject *parent):
    QTcpServer(parent),
    serverIPAddress(serverIPAddress)
{
    configureStatisticsCounter();
    configureLogSystem();
    configureTimers();
    // Создаём потоки после выделения всех ресурсов
    initWorkers();
}

MultithreadTcpServer::~MultithreadTcpServer()
{
    removeWorkers();
    delete statisticsCounter;
}

void MultithreadTcpServer::updateWorkingTimeCounter()
{
    currentSessionWorkingTime.addSecond();
    emit workingTimeUpdated(currentSessionWorkingTime.toString());
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

void MultithreadTcpServer::configureLogSystem()
{
    BesLogSystem *logSystem = BesLogSystem::getInstance();
    /*
     * Все сообщения, которые мы хотим журналировать, мы должны отправлять в
     * виде сигналов. Эти сигналы будут обрабатываться логгирующей системой
     */

    connect(this, SIGNAL(started()),
            logSystem, SLOT(logServerStartedMessage()));
    connect(this, SIGNAL(stopped()),
            logSystem, SLOT(logServerStoppedMessage()));

    connect(this, SIGNAL(clientConnectionOpenned()),
            logSystem, SLOT(logClientConnectionCreatedMessage()));
    connect(this, SIGNAL(clientConnectionClosed()),
            logSystem, SLOT(logClientConnectionClosedMessage()));
}

void MultithreadTcpServer::configureTimers()
{
    currentSessionWorkingTimeTimer.setInterval(WORKING_TIME_COUNTER_UPDATE_TIME);
    connect(&currentSessionWorkingTimeTimer, SIGNAL(timeout()),
            SLOT(updateWorkingTimeCounter()));
}

void MultithreadTcpServer::start()
{
    // Получаем доступ к настройкам конфигурации
    BesConfigReader *configs = BesConfigReader::getInstance();

    // Начинаем слушать входящие соединения
    listen(serverIPAddress, configs->getInt("server","client_connection_port"));
    // Запускает рабочие потоки
    for(ServerWorker *worker:serverWorkers)
    {
        worker->start(QThread::Priority::TimeCriticalPriority);
    }
    // Запускаем счётчик времени работы
    currentSessionWorkingTimeTimer.start();
    // Сообщаем, что сервер начал свою работу
    emit started();
}

void MultithreadTcpServer::stop()
{
    // Приостанавливаем прослушивание входящих соединений
    close();
    // Останавливаем счётчик времени работы сервера
    currentSessionWorkingTimeTimer.stop();
    // Отправляем сигнал о том, что нужно остановить рабочие потоки (отключаем все соединения от них)
    emit stopped();
    // Сейчас наиболее безопасно обновить параметры конфигурации, т.к. никакая из частей системы
    // не будет обращаться к данным конфигурации(до перезапуска)
    BesConfigReader::getInstance()->readConfigs();
    ClientConnection::initSslConfiguration();
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
    /*
     * Статический метод QThread::idealThreadCount() выдает наилучшее количество потоков для системы.
     *
     * Однако, если определить его невозможно, он выдаст единицу.
     * В таком случае мы будем запускать 4 потока.
     */
    if(workerThreadsNumber == 1)
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
        connect(this, SIGNAL(stopped()), newWorker, SLOT(quit()));
        // Пробрасываем сигнал о разрыве клиентского соединения "во вне"
        connect(newWorker, SIGNAL(clientConnectionClosed()), SIGNAL(clientConnectionClosed()));
        // Пробрасываем сигнал о регистрации сообщения в журнале сообщений
        serverWorkers.append(newWorker);
    }
}

void MultithreadTcpServer::incomingConnection(qintptr socketDescriptor)
{
    /*
     * Балансируем нагрузку между рабочими потоками следующим образом:
     * Новое соединение начинает обрабатывать наименее загруженный поток
     */

    // Номер потока, который будет обрабатывать данное подключение
    int handlingThreadNumber=0;
    for(int i{1}; i < serverWorkers.length(); i++)
    {
        // Находим наименее загруженный поток
        if(serverWorkers[i]->getHandlingConnectionsCounter() < serverWorkers[handlingThreadNumber]->getHandlingConnectionsCounter())
        {
            handlingThreadNumber=i;
        }
    }

    qDebug() << QString("Получено новое соединение|Новое подключение обрабатывается потоком %1").arg(handlingThreadNumber);
    // Выбираем серверный рабочий поток, который будет ответственен за обработку сообщений от нового подключения
    ServerWorker *currentWorker = serverWorkers[handlingThreadNumber];
    currentWorker->addClientConnection(socketDescriptor);
    emit clientConnectionOpenned();
}



