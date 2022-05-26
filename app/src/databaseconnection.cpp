#include "databaseconnection.h"
#include "beslogsystem.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>

DatabaseConnection::DatabaseConnection(const QString &connectionName)
{
    this->connectionName=connectionName;
    configureDatabaseConnection();
}

DatabaseConnection::~DatabaseConnection()
{
    // Перед удалением объекта подключения к базе данных, нужно удалить физическое подключение
    besMesDatabase.close();
    besMesDatabase=QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}

void DatabaseConnection::configureDatabaseConnection()
{
    // Получаем доступ к параметрам конфигурации
    ConfigReader &config_reader = ConfigReader::getInstance();
    besMesDatabase = QSqlDatabase::addDatabase(USING_PLUGIN_NAME, connectionName);
    // Указываем ip-адрес устройства, на котором развёрнута база данных и порт, прослушиваемый базой данных
    besMesDatabase.setHostName(config_reader.getDatabaseHostAddress());
    besMesDatabase.setPort(config_reader.getDatabaseListeningPort());
    // Указываем имя базы данных, к которой хотим подключиться
    besMesDatabase.setDatabaseName(config_reader.getDatabaseName());
    // Указываем имя аккаунта, который мы будем использовать для подключения к базе данных
    besMesDatabase.setUserName(config_reader.getDatabaseUserName());
    besMesDatabase.setPassword(config_reader.getDatabaseUserPassword());
}

void DatabaseConnection::open()
{
    if(!besMesDatabase.open())
    {
        BesLogSystem::getInstance()->logUnableEstablishDatabaseConnection(connectionName,
                                                                          besMesDatabase.lastError().text());
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
