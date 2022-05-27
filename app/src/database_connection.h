#ifndef DATABASE_CONNECTION_H
#define DATABASE_CONNECTION_H

#include <QSqlDatabase>
#include "config_reader.h"

#define USING_PLUGIN_NAME "QPSQL"
#define USER_TABLE_NAME "user"

class DatabaseConnection
{
public:
    DatabaseConnection(const QString &connectionName);
    ~DatabaseConnection();
    /// Открывате физическое подключение с базой данных
    void open();
    /// Закрывает соединение с базой данных, освобождая все ресурсы
    void close();

    /// Проверяет, существует ли пользователь с таким адресом электронной почты
    bool isEmailAvailable(const QString &email);
    /// Проверяет, существует ли пользователь с таким адресом электронной почты
    /// и паролем
    bool verifyLogIn(const QString &email, const QString &password);
    /// Добавляет нового пользователя в базу данных
    bool registerNewUser(const QString &firstName, const QString &lastName,
                    const QString &email, const QString &password);
    /// Проверяет, является ли подключение к базе даннzых активным
    bool isActive();
private:
    /// Настраивает подключение к базе данных
    /// connectionName - имя подключения к базе данных. Должно быть уникальным
    void configureDatabaseConnection();
    /// Имя подключения к базе данных
    QString connectionName;
    /// Подключение к базе данных проекта BesMes
    QSqlDatabase besMesDatabase;
};

#endif // DATABASE_CONNECTION_H