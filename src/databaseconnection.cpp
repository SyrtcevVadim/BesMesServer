#include "databaseconnection.h"
#include <QDebug>
#include <QSqlError>

DatabaseConnection::DatabaseConnection(const QString &connectionName)
{
    besMesDatabase = QSqlDatabase::addDatabase(USING_PLUGIN, connectionName);
}

void DatabaseConnection::setDatabaseName(const QString &databaseName)
{
    besMesDatabase.setDatabaseName(databaseName);
}

void DatabaseConnection::setDatabaseAddress(const QString &databaseAddress, const int port)
{
    besMesDatabase.setHostName(databaseAddress);
    besMesDatabase.setPort(port);
}

void DatabaseConnection::setUser(const QString &userName, const QString &password)
{
    besMesDatabase.setUserName(userName);
    besMesDatabase.setPassword(password);
}

void DatabaseConnection::open()
{
    if(besMesDatabase.open())
    {
        qDebug() << "Подключение к базе данных успешно пройдено!";
    }
    else
    {
        qDebug() << "Ошибка "<< besMesDatabase.lastError().text();
    }
}
