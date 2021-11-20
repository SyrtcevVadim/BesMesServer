#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include<QSqlDatabase>
#include"user.h"

#define USING_PLUGIN "QMYSQL"
#define DEFAULT_DATABASE "besmesdb"
#define USER_TABLE "user"

class DatabaseConnection
{
public:
    DatabaseConnection(const QString &connectionName);
    /// Устанавливает имя используемой базы данных
    void setDatabaseName(const QString &databaseName=DEFAULT_DATABASE);
    /// Устанавливает данные об адресе и порте устройства, на котором находится база данных
    void setDatabaseAddress(const QString &databaseAddress, const int port);
    /// Устанавливает данные об используемом аккаунте в базе данных
    void setUser(const QString &userName, const QString &password);
    /// Открывате физическое подключение с базой данных
    void open();
    /// Проверяет, существует ли пользователь с таким адресом электронной почты
    bool userExists(const QString &email);
    /// Проверяет, существует ли пользователь с таким адресом электронной почты
    /// и паролем
    bool userExists(const QString &email, const QString &password);
    /// Добавляет нового пользователя в базу данных
    bool addNewUser(const QString &firstName, const QString &lastName,
                    const QString &email, const QString &password);
    /// Закрывает соединение с базой данных, освобождая все ресурсы
    void close();
    /// Проверяет, является ли подключение к базе данных активным
    bool isActive();
private:
    /// Подключение к базе данных проекта BesMes
    QSqlDatabase besMesDatabase;
};

#endif // DATABASECONNECTION_H
