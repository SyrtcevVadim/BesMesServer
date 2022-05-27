#include<QThread>
#include<QTcpSocket>
#include "multithread_tcp_server.h"
#include "client_connection.h"


int MultithreadTcpServer::workerThreadsNumber = QThread::idealThreadCount();

MultithreadTcpServer::MultithreadTcpServer(QHostAddress serverIPAddress,
                                           QObject *parent):
    QTcpServer(parent),
    serverIPAddress(serverIPAddress)
{
    // Создаём потоки после выделения всех ресурсов
    initWorkers();
}

MultithreadTcpServer::~MultithreadTcpServer()
{
    removeWorkers();
}

void MultithreadTcpServer::start()
{
    // Получаем доступ к настройкам конфигурации
    ConfigReader &config_reader = ConfigReader::getInstance();
    qDebug() << config_reader.getServerListeningPort();

    // Начинаем слушать входящие соединения
    listen(serverIPAddress, config_reader.getServerListeningPort());
    // Запускает рабочие потоки
    for(ServerWorker *worker:serverWorkers)
    {
        worker->start(QThread::Priority::TimeCriticalPriority);
    }
    // Сообщаем, что сервер начал свою работу
    emit started();
}

void MultithreadTcpServer::stop()
{
    // Приостанавливаем прослушивание входящих соединений
    close();
    // Отправляем сигнал о том, что нужно остановить рабочие потоки (отключаем все соединения от них)
    emit stopped();
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
