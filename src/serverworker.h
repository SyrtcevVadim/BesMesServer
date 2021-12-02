#ifndef SERVERWORKER_H
#define SERVERWORKER_H
// Автор: Сырцев Вадим Игоревич
#include<QRandomGenerator>
#include "databaseconnection.h"
#include "besconfigeditor.h"
#include "beslogsystem.h"
#include <QThread>

/**
 * Описывает поток сервера, в котором могут обрабатываться пользовательские подключения
 */
class ServerWorker : public QThread
{
    Q_OBJECT
public:
    ServerWorker(BesConfigEditor *serverConfigEditor,
                 BesConfigEditor *databaseConnectionConfigEditor,
                 BesConfigEditor *emailSenderConfigEditor,
                 BesLogSystem *logSystem,
                 QObject *parent = nullptr);
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
    /// Сигнал, высылаемый, когда рабочий поток не смог установить соединение с
    /// базой данных
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
    /// Сигнал, высылаемый, когда пользователь для регистрации указал
    /// занятую почту
    void clientUsedOccupiedEmailForRegistration(QString email);


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
    /// Обрабатывает команду авторизации администратора
    void processSuperLogInCommand(QString login, QString password);
    /// Уменьшает счётчик клиентских соединений, обрабатываемых текущим потоком
    void decreaseHandlingConnectionsCounter();
private:
    inline void initCounters();
    /// Генерирует код верификации регистрации
    QString generateVerificationCode();
    /// Настраивает подключение к базе данных в рабочем потоке
    void configureDatabaseConnection();
    /// Настраивает систему логгирования сообщений из рабочих потоков
    void configureLogSystem();

    /// Счётчик созданных объектов
    static unsigned int createdObjectCounter;
    /// Номер потока, работающего в рамках серверного приложения
    unsigned int id;
    /// Счётчик соединений, обрабатываемых данным рабочим потоком
    unsigned long long handlingConnectionsCounter;
    /// Объект подключения к базе данных
    DatabaseConnection *databaseConnection;

    /// Обрабатывает параметры сервера
    BesConfigEditor *serverConfigEditor;
    /// Обрабатывает параметры подключения к базе данных
    BesConfigEditor *databaseConnectionConfigEditor;
    /// Обрабатывает параметры отправителя email-писем
    BesConfigEditor *emailSenderConfigEditor;
    /// Система журналирования сообщений
    BesLogSystem *logSystem;
    /// Генератор псевдослучайных чисел. Используется для генерации
    /// кодов верификации
    static QRandomGenerator generator;
};

#endif // SERVERWORKER_H
