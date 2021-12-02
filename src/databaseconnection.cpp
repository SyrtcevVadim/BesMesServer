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
    QString strQuery = QString("SELECT email FROM %1 WHERE email='%2'").arg(USER_TABLE, email);

    query.exec(strQuery);
    // Если пользователя с таким адресом эл. почты не существует
    while(query.next())
    {
        return true;
    }
    return false;
}

bool DatabaseConnection::userExists(const QString &email, const QString &password)
{
    QSqlQuery query(besMesDatabase);
    QString strQuery = QString("SELECT email, password FROM %1 WHERE email='%2' AND password='%3'").
            arg(USER_TABLE, email, password);
    query.exec(strQuery);

    while(query.next())
    {
        return true;
    }
    return false;
}

bool DatabaseConnection::addNewUser(const QString &firstName, const QString &lastName, const QString &email, const QString &password)
{
    QSqlQuery query(besMesDatabase);
    //TODO СДЕЛАТЬ ПОДДЕРЖКУ КИРИЛЛИЦЫ
    QString queryString = QString("INSERT INTO %1 (first_name, last_name, email, password, registration_date) "
                    "VALUES ('%2', '%3', '%4', '%5', CURDATE())")
            .arg(USER_TABLE,
                 firstName,
                 lastName,
                 email,
                 password);

    qDebug() << queryString;
    bool result =query.exec(QObject::trUtf8(queryString.toStdString().c_str()));
    qDebug() << query.lastError();
    return result;
}

bool DatabaseConnection::isActive()
{
    return besMesDatabase.isOpen();
}
