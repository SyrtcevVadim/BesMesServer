#include "databaseconnection.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>

DatabaseConnection::DatabaseConnection(const QString &connectionName)
{
    besMesDatabase = QSqlDatabase::addDatabase(USING_PLUGIN, connectionName);
}

void DatabaseConnection::close()
{
    besMesDatabase.close();
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
    if(!besMesDatabase.open())
    {
        qDebug() << "Ошибка "<< besMesDatabase.lastError().text();
    }
}

bool DatabaseConnection::userExists(const QString &email)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT email FROM :user_table");
    query.bindValue(0, USER_TABLE);
    query.exec();
    // Если пользователя с таким адресом эл. почты не существует
    while(query.next())
    {
        // Сравниваем кажду почту с предоставленной
        if(query.value(0).toString() == email)
        {
            return true;
        }
    }
    return false;
}

bool DatabaseConnection::userExists(const QString &email, const QString &password)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT email,password FROM :user_table");
    query.bindValue(0, USER_TABLE);
    query.exec();
    // Если пользователя с таким адресом эл. почты не существует
    while(query.next())
    {
        // Сравниваем кажду почту с предоставленной
        if(query.value(0).toString() == email && query.value(1).toString()==password)
        {
            return true;
        }
    }
    return false;
}

bool DatabaseConnection::addNewUser(const QString &firstName, const QString &lastName, const QString &email, const QString &password)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("INSERT INTO :user_table (first_name, last_name, email, password, registration_date)"
                    "VALUES (:first_name, :last_name, :email, :password, :registration_date)");
    query.bindValue(0, firstName);
    query.bindValue(1, lastName);
    query.bindValue(2, email);
    query.bindValue(3, password);
    query.bindValue(4, QDate::currentDate());
    return query.exec();
}
