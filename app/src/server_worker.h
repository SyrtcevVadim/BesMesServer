#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H
// Автор: Сырцев Вадим Игоревич
#include<QRandomGenerator>
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
    /// Сигнал, высылаемый при успешном прохождении пользователем аутентификации
    void clientLoggedIn(QString email);
    /// Сигнал, высылаемый при неудачной попытке прохождения аутентификации
    void clientFailedAuthentication(QString email);
    /// Сигнал, высылаемый после отправки пользователю кода верификации
    void verificationCodeWasSent(QString email);
    /// Сигнал, высылаемый после успешной регистрации пользователя
    void clientWasRegistered(QString email);
    /// Сигнал, высылаемый, когда пользователь отправляет неверный код верификации
    void clientSentWrongVerificationCode(QString email,QString code);
    /// Сигнал, высылаемый, когда пользователь для регистрации указал занятую почту
    void clientUsedOccupiedEmailForRegistration(QString email);
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
    /// Генератор псевдослучайных чисел. Используется для генерации
    /// кодов верификации
    static QRandomGenerator generator;
};

#endif // SERVER_WORKER_H
