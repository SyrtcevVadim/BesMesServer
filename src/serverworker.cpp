#include <QDebug>
#include "serverworker.h"
#include "clientconnection.h"


// Изначально нет созданных объектов серверных рабочих
unsigned int ServerWorker::createdObjectCounter = 0;

ServerWorker::ServerWorker(const QString &databaseAddress,
                           const int databasePort,
                           const QString &userName,
                           const QString &password,
                           QObject *parent):
    QThread(parent)
{

    id = createdObjectCounter++;
    initCounters();
    // Открываем соединение с базой данных
    dbConnection = new DatabaseConnection(QString("%1%2").arg(userName, QString().setNum(id)));
    dbConnection->setDatabaseAddress(databaseAddress, databasePort);
    dbConnection->setUser(userName, password);
    dbConnection->setDatabaseName();

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
    // Обрабатываем команду приветствия
    connect(incomingConnection, SIGNAL(helloSaid(QString,QString)),
            SLOT(processHelloMessage(QString,QString)));
    // При остановке рабочего потока должны быть разорваны все пользовательские соединения
    connect(this, SIGNAL(stopWorker()),
            incomingConnection, SLOT(close()));

    incomingConnection->processServerResponse("Hello! Say the name and the pass\r\n");
}

void ServerWorker::decreaseHandlingConnectionsCounter()
{
    --handlingConnectionsCounter;
    qDebug() << QString("Клиент в потоке %1 разорвал соединение. На данном потоке осталось %2 клиентов")
                .arg(QString().setNum(id), QString().setNum(handlingConnectionsCounter));
}

void ServerWorker::processHelloMessage(QString userName, QString password)
{
    qDebug() << QString("Пользователь %1 сказал привет!").arg(userName);
    ClientConnection *client = (ClientConnection*)sender();
    // TODO Проверяем, есть ли такой пользователь в БД
    client->processServerResponse(QString("SUCCESS you were logged in\r\n"));

}

void ServerWorker::run()
{
    qDebug() << QString("Поток %1 запущен").arg(id);
    dbConnection->open();
    exec();
}

void ServerWorker::initCounters()
{
    handlingConnectionsCounter=0;
}
