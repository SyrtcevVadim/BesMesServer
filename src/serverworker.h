#ifndef SERVERWORKER_H
#define SERVERWORKER_H
// Автор: Сырцев Вадим Игоревич
#include<QRandomGenerator>
#include "databaseconnection.h"
#include "besconfigeditor.h"
#include <QThread>

/**
 * Описывает поток сервера, в котором могут обрабатываться пользовательские подключения
 */
class ServerWorker : public QThread
{
    Q_OBJECT
public:
    ServerWorker(BesConfigEditor *databaseConnectionConfigEditor,
                 BesConfigEditor *emailSenderConfigEditor,
                 QObject *parent = nullptr);
    /// Добавляет в данный серверный поток обработки новое клиентское
    /// соединение
    void addClientConnection(qintptr socketDescriptor);
    /// Возвращает количество клиентских подключений, обрабатываем текущим рабочим потоком
    unsigned long long getHandlingConnectionsCounter();

signals:
    /// Сигнал, высылаемый, когда клиентское соединение, обрабатываемое
    /// в данном потоке, разрывается
    void clientConnectionClosed();
    /// Сигнал о намерении сделать запись в журнале (логе)
    void logMessage(QString message);
protected:
    void run();
private slots:
    /// Обрабатывает команду аутентификации
    void processLogInCommand(QString email, QString password);
    /// Обрабатывает команду регистрации
    void processRegistrationCommand(QString firstName, QString last_name,
                                    QString email, QString password);
    /// Обрабатывает команду верификации регистрации
    void processVerificationCommand(QString code);
    /// Уменьшает счётчик клиентских соединений, обрабатываемых текущим потоком
    void decreaseHandlingConnectionsCounter();
private:
    inline void initCounters();
    /// Генерирует код верификации регистрации
    QString generateVerificationCode();

    /// Настраивает подключение к базе данных в рабочем потоке
    void configureDBConnection();
    /// Счётчик созданных объектов
    static unsigned int createdObjectCounter;
    /// Номер потока, работающего в рамках серверного приложения
    unsigned int id;
    /// Счётчик соединений, обрабатываемых данным рабочим потоком
    unsigned long long handlingConnectionsCounter;
    /// Объект подключения к базе данных
    DatabaseConnection *dbConnection;
    /// Обрабатывает параметры подключения к базе данных
    BesConfigEditor *databaseConnectionConfigEditor;
    /// Обрабатывает параметры отправителя email-писем
    BesConfigEditor *emailSenderConfigEditor;
    /// Генератор псевдослучайных чисел. Используется для генерации
    /// кодов верификации
    static QRandomGenerator generator;
};

#endif // SERVERWORKER_H
