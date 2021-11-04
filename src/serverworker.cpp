#include <QDebug>
#include "serverworker.h"
#include "clientconnection.h"


// Изначально нет созданных объектов серверных рабочих
unsigned int ServerWorker::createdObjectCounter = 0;

ServerWorker::ServerWorker(QObject *parent): QThread(parent)
{
    id = createdObjectCounter++;
}

void ServerWorker::addClientConnection(qintptr socketDescriptor)
{
    ClientConnection *incomingConnection = new ClientConnection(socketDescriptor);
    // После разрыва пользовательского соединения уведомляем об этом рабочий поток
    connect(incomingConnection, SIGNAL(closed()), SIGNAL(clientConnectionClosed()));
    connect(incomingConnection, SIGNAL(helloSaid(QString,QString)),
            SLOT(processHelloMessage(QString,QString)));
    // При остановке рабочего потока должны быть разорваны все пользовательские соединения
    connect(this, SIGNAL(stopWorker()),
            incomingConnection, SLOT(close()));

    incomingConnection->receiveServerResponse("Hello! Say the name and the pass pls\r\n");
}

void ServerWorker::processHelloMessage(QString userName, QString password)
{
    emit logMessage(QString("Пользователь %1 сказал привет!").arg(userName));
}

void ServerWorker::run()
{
    emit logMessage(QString("Поток %1 запущен!").arg(id));
    exec();
}
