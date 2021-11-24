#include<thread>
#include<QTcpSocket>
#include "multithreadtcpserver.h"


int MultithreadTcpServer::workerThreadsNumber = std::thread::hardware_concurrency();

MultithreadTcpServer::MultithreadTcpServer(QHostAddress serverIPAddress,
                                           BesConfigEditor *serverConfigEditor,
                                           BesConfigEditor *databaseConnectionConfigEditor,
                                           BesConfigEditor *emailSenderConfigEditor,
                                           QObject *parent):
    QTcpServer(parent),
    databaseConnectionConfigEditor(databaseConnectionConfigEditor),
    serverConfigEditor(serverConfigEditor),
    emailSenderConfigEditor(emailSenderConfigEditor),
    serverIPAddress(serverIPAddress)
{
    initWorkers();
    configureStatisticsCounter();
    configureLogSystem();

    currentSessionWorkingTimeTimer.setInterval(WORKING_TIME_COUNTER_UPDATE_TIME);
    connect(&currentSessionWorkingTimeTimer, SIGNAL(timeout()),
            SLOT(updateWorkingTimeCounter()));
}

MultithreadTcpServer::~MultithreadTcpServer()
{
    removeWorkers();
    delete statisticsCounter;
    delete logSystem;
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
    logSystem = new LogSystem("latest.txt", this);
    connect(this, SIGNAL(logMessage(QString)),
            logSystem, SLOT(logToFile(QString)));
}

void MultithreadTcpServer::start()
{
    // Начинаем слушать входящие соединения
    listen(serverIPAddress, serverConfigEditor->getInt("port"));
    // Запускает рабочие потоки
    for(ServerWorker *worker:serverWorkers)
    {
        worker->start(QThread::Priority::TimeCriticalPriority);
    }
    // Запускаем счётчик времени работы
    currentSessionWorkingTimeTimer.start();
    // Сообщаем, что сервер начал свою работу
    emit started();
    emit logMessage("Сервер включён");

}

void MultithreadTcpServer::stop()
{
    // Приостанавливаем прослушивание входящих соединений
    close();
    // Останавливаем счётчик времени работы сервера
    currentSessionWorkingTimeTimer.stop();
    // Отправляем сигнал о том, что нужно остановить рабочие потоки (отключаем все соединения от них)
    emit stopped();
    emit logMessage("Сервер отключён");
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
        ServerWorker *newWorker = new ServerWorker(databaseConnectionConfigEditor,
                                                   emailSenderConfigEditor,this);
        /* Когда работа сервера останавливается, рабочим потокам отправляется сигнал
         * Мы отправляем именно сигнал, а не слот, поскольку рабочий поток не хранит объекты подключений в коллекции
         * Объекты подключения, получив данный сигнал, разрывают своё соединение с сервером. Таким образом, нагрузка на рабочий поток
         * останавливается. Сервер после этого можно считать простаивающим
         */
        connect(this, SIGNAL(stopped()), newWorker, SLOT(quit()));
        connect(newWorker, SIGNAL(logMessage(QString)), SIGNAL(logMessage(QString)));
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



