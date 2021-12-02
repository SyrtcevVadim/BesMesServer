#include "databaseconnection.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>

DatabaseConnection::DatabaseConnection(const QString &connectionName)
{
    this->connectionName=connectionName;
    configureConfigEditors();
    configureDatabaseConnection();
}

DatabaseConnection::~DatabaseConnection()
{
    // Перед удалением объекта подключения к базе данных, нужно удалить физическое подключение
    besMesDatabase.close();
    besMesDatabase=QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
    delete databaseConnectionConfigEditor;
}

void DatabaseConnection::configureConfigEditors()
{
    databaseConnectionConfigEditor=new BesConfigEditor(DATABASE_CONFIG_FILE_NAME);
}

void DatabaseConnection::configureDatabaseConnection()
{
    besMesDatabase = QSqlDatabase::addDatabase(USING_PLUGIN_NAME, connectionName);
    // Указываем ip-адрес устройства, на котором развёрнута база данных и порт, прослушиваемый базой данных
    besMesDatabase.setHostName(databaseConnectionConfigEditor->getString("address"));
    besMesDatabase.setPort(databaseConnectionConfigEditor->getInt("port"));
    // Указываем имя базы данных, к которой хотим подключиться
    besMesDatabase.setDatabaseName(databaseConnectionConfigEditor->getString("databaseName"));
    // Указываем имя аккаунта, который мы будем использовать для подключения к базе данных
    besMesDatabase.setUserName(databaseConnectionConfigEditor->getString("userName"));
    besMesDatabase.setPassword(databaseConnectionConfigEditor->getString("password"));
}

void DatabaseConnection::open()
{
    if(!besMesDatabase.open())
    {
        qDebug() << "Ошибка "<< besMesDatabase.lastError().text();
    }
    else
    {
        qDebug() << "Успешно установили подключение к базе данных";
    }
}

void DatabaseConnection::close()
{
    besMesDatabase.close();
}

bool DatabaseConnection::userExists(const QString &email)
{
    QSqlQuery query(besMesDatabase);
    QString strQuery = QString("SELECT email FROM %1 WHERE email='%2'").arg(USER_TABLE_NAME, email);

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
            arg(USER_TABLE_NAME, email, password);
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
            .arg(USER_TABLE_NAME,
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
