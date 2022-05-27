#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H
// Автор: Сырцев Вадим Игоревич
#include "database_connection.h"
#include "config_reader.h"
#include <QThread>

/**
 * Описывает поток сервера, в котором могут обрабатываться пользовательские подключения
 */
class ServerWorker : public QThread
{
    Q_OBJECT
public:
    ServerWorker(QObject *parent = nullptr);
    ~ServerWorker();
    /// Добавляет в данный серверный поток обработки новое клиентское
    /// соединение
    void addClientConnection(qintptr socketDescriptor);
    /// Возвращает количество клиентских подключений, обрабатываем текущим рабочим потоком
    unsigned long long getHandlingConnectionsCounter();
    // Запросы от пользователей
    bool verify_log_in(const QString &email, const QString &password);
    bool register_new_user(const QString &firstName, const QString &lastName, const QString &email, const QString &password);
public slots:
    void quit();

signals:
    /// Сигнал, высылаемый, когда рабочему потоку удалось установить соединение
    /// с базой данных
    void databaseConnectionEstablished(int workerId);
    /// Сигнал, высылаемый, когда рабочий поток не смог установить соединение с базой данных
    void databaseConnectionFailed(int workerId);
    /// Сигнал, высылаемый, когда клиентское соединение, обрабатываемое
    /// в данном потоке, разрывается
    void clientConnectionClosed();

protected:
    void run();
private slots:
    /// Уменьшает счётчик клиентских соединений, обрабатываемых текущим потоком
    void decreaseHandlingConnectionsCounter();
private:
    inline void initCounters();
    /// Настраивает подключение к базе данных в рабочем потоке
    void configureDatabaseConnection();
    /// Счётчик созданных объектов
    static unsigned int createdObjectCounter;
    /// Номер потока, работающего в рамках серверного приложения
    unsigned int id;
    /// Счётчик соединений, обрабатываемых данным рабочим потоком
    unsigned long long handlingConnectionsCounter;
    /// Объект подключения к базе данных
    DatabaseConnection *databaseConnection;
};

#endif // SERVER_WORKER_H
