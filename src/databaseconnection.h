#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include<QSqlDatabase>


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
private:
    /// Подключение к базе данных проекта BesMes
    QSqlDatabase besMesDatabase;
};

#endif // DATABASECONNECTION_H
