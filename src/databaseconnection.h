#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QSqlDatabase>
#include "besconfigeditor.h"

#define USING_PLUGIN_NAME "QMYSQL"
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
    bool userExists(const QString &email);
    /// Проверяет, существует ли пользователь с таким адресом электронной почты
    /// и паролем
    bool userExists(const QString &email, const QString &password);
    /// Добавляет нового пользователя в базу данных
    bool addNewUser(const QString &firstName, const QString &lastName,
                    const QString &email, const QString &password);

    /// Проверяет, является ли подключение к базе данных активным
    bool isActive();
private:
    /// Настраивает менеджеры конфигурационных файлов
    void configureConfigEditors();
    /// Настраивает подключение к базе данных
    /// connectionName - имя подключения к базе данных. Должно быть уникальным
    void configureDatabaseConnection();
    /// Имя подключения к базе данных
    QString connectionName;
    /// Подключение к базе данных проекта BesMes
    QSqlDatabase besMesDatabase;
    /// Менеджер конфигурационного файла, хранящего настройки для подключения к базе данных
    BesConfigEditor *databaseConnectionConfigEditor;
};

#endif // DATABASECONNECTION_H
