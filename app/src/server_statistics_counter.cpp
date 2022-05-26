#include "server_statistics_counter.h"

ServerStatisticsCounter::ServerStatisticsCounter(QObject *parent):
    QThread(parent)
{
    activeConnectionsCounter=0;
    totalEstablishedConnectionsCounter=0;
}

void ServerStatisticsCounter::run()
{
    exec();
}

void ServerStatisticsCounter::increaseActiveConnectionsCounter()
{
    activeConnectionsCounter++;
    emit activeConnectionsCounterChanged(activeConnectionsCounter);
}

void ServerStatisticsCounter::decreaseActiveConnectionsCounter()
{
    activeConnectionsCounter--;
    emit activeConnectionsCounterChanged(activeConnectionsCounter );
}


void ServerStatisticsCounter::increaseTotalEstablishedConnectionsCounter()
{
     totalEstablishedConnectionsCounter++;
}

unsigned long long ServerStatisticsCounter::getTotalEstablishedConnectionsCounter()
{
    return totalEstablishedConnectionsCounter;
}
