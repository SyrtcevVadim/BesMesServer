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
    connect(incomingConnection, SIGNAL(helloSaid(QString, QString)),
            SLOT(processHelloMessage(QString, QString)));
    incomingConnection->receiveServerResponse("Hello! Say the name and the pass pls\r\n");
}

void ServerWorker::processHelloMessage(QString userName, QString password)
{
    qDebug() << "Пользователь " << userName <<"сказал привет!";
}

void ServerWorker::run()
{
    qDebug() << QString("Поток %1 запущен!").arg(id);

    exec();
}
