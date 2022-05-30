#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H
// Автор: Сырцев Вадим Игоревич
#include <QThread>
#include "database_connection.h"
#include "config_reader.h"
#include "user.h"
#include "chat.h"

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
    bool verifyLogIn(const QString &email, const QString &password);
    qint64 getUserId(const QString &email);
    bool registerNewUser(const QString &firstName, const QString &lastName, const QString &email, const QString &password);
    QVector<User> getListOfUsers();
    QVector<Chat> getListOfChats(qint64 userId);
    bool createNewChat(qint64 ownerId, const QString &chatTitle);
    bool inviteToChat(qint64 chatId, qint64 userId);
    bool kickFromChat(qint64 chatId, qint64 userId);
    QVector<qint64> getUsersInChat(qint64 chatId);
    bool sendMessage(qint64 chatId, const QString &messageBody, qint64 senderId);

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
