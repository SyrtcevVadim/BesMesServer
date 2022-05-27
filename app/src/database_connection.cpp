#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>
#include <QCryptographicHash>
#include "database_connection.h"

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
        qDebug() << "Не смогли открыть подключение к базе данных";
    }
}

void DatabaseConnection::close()
{
    besMesDatabase.close();
}

bool DatabaseConnection::isEmailAvailable(const QString &email)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT is_email_available(:email)");
    query.bindValue(":email", email);
    query.exec();
    query.first();
    // Возвращает true, если данная почта свободна, иначе - false
    return query.value(0).toBool();
}

bool DatabaseConnection::verifyLogIn(const QString &email, const QString &password)
{
    // Хэшируем пароль
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Algorithm::Keccak_512);

    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT verify_log_in(:email, :hashed_password)");
    query.bindValue(":email", email);
    query.bindValue(":hashed_password", QLatin1String(hashedPassword.toHex()));
    query.exec();
    query.first();
    return query.value(0).toBool();
}

bool DatabaseConnection::registerNewUser(const QString &firstName, const QString &lastName, const QString &email, const QString &password)
{
    // Хэшируем пароль
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Algorithm::Keccak_512);
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT register_new_user(:firstName, :lastName, :email, :hashed_password)");
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":email", email);
    query.bindValue(":hashed_password", QLatin1String(hashedPassword.toHex()));
    query.exec();
    query.first();
    return query.value(0).toBool();
}

bool DatabaseConnection::isActive()
{
    return besMesDatabase.isOpen();
}
