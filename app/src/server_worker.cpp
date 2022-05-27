#include <QDebug>
#include "server_worker.h"
#include "client_connection.h"

// Изначально нет созданных объектов серверных рабочих
unsigned int ServerWorker::createdObjectCounter = 0;
QRandomGenerator ServerWorker::generator(QDateTime::currentSecsSinceEpoch());

ServerWorker::ServerWorker(QObject *parent):
    QThread(parent)
{
    id = createdObjectCounter++;
    initCounters();
}

ServerWorker::~ServerWorker()
{
    qDebug() << "Удаляем рабочего";
    delete databaseConnection;
}

void ServerWorker::quit()
{
    // При остановке рабочего потока нужно удалить соединение с базой данных
    delete databaseConnection;
    QThread::quit();
}

void ServerWorker::configureDatabaseConnection()
{
    databaseConnection = new DatabaseConnection(QString("connection%1")
                                           .arg(id));
}

unsigned long long ServerWorker::getHandlingConnectionsCounter()
{
    return handlingConnectionsCounter;
}

void ServerWorker::addClientConnection(qintptr socketDescriptor)
{
    ++handlingConnectionsCounter;
    ClientConnection *incomingConnection = new ClientConnection(socketDescriptor);
    // Связываем клиентское подключение с рабочим потоком
    // После разрыва пользовательского соединения уведомляем об этом рабочий поток
    connect(incomingConnection, SIGNAL(closed()), SIGNAL(clientConnectionClosed()));
    // После разрыва пользовательского соединения уменьшаем счётчик
    connect(incomingConnection, SIGNAL(closed()), SLOT(decreaseHandlingConnectionsCounter()));
    // После разрыва соединения с клиентским приложением, нужно удалить объект
    connect(incomingConnection, SIGNAL(closed()), incomingConnection, SLOT(deleteLater()));

    // При остановке рабочего потока должны быть разорваны все пользовательские соединения
    connect(this, SIGNAL(finished()),
            incomingConnection, SLOT(close()));
}

void ServerWorker::decreaseHandlingConnectionsCounter()
{
    --handlingConnectionsCounter;
    qDebug() << QString("Клиент в потоке %1 разорвал соединение. На данном потоке осталось %2 клиентов")
                .arg(QString().setNum(id), QString().setNum(handlingConnectionsCounter));
}

void ServerWorker::run()
{
    qDebug() << QString("Поток %1 запущен").arg(id);
    //configureDatabaseConnection();
    //databaseConnection->open();
    exec();
}

void ServerWorker::initCounters()
{
    handlingConnectionsCounter=0;
}

