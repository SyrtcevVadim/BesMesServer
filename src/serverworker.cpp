#include "serverworker.h"
#include <QDebug>

// Изначально нет созданных объектов серверных рабочих
unsigned int ServerWorker::createdObjectCounter = 0;

ServerWorker::ServerWorker(QObject *parent): QThread(parent)
{
    id = createdObjectCounter++;
}


void ServerWorker::run()
{
    qDebug() << QString("Поток %1 запущен!").arg(id);
}
