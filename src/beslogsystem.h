#ifndef BESLOGSYSTEM_H
#define BESLOGSYSTEM_H
// Автор: Сырцев Вадим Игоревич
#include "logsystem.h"
#include <QTime>
#include <QTimer>
#include <mutex>

using std::mutex;
using std::lock_guard;

// Интервал, через который нужно проверять, закончился ли день или нет
#define CHECK_END_OF_DAY_INTERVAL 1000
// Устанавливаем время, в которое нужно сохранять журнал сообщений за
// предыдущий день. По умолчанию это 23.59.59
#define END_OF_DAY_TIME QTime(23,59,59)

/// ВАЖНО: этот класс реализует паттерн проектирования "Одиночка"
class BesLogSystem : public LogSystem
{
    Q_OBJECT
public:
    // Одиночку нельзя клонировать
    BesLogSystem(BesLogSystem &) = delete;
    void operator=(const BesLogSystem &) = delete;
    /// Даёт доступ к объекту логгирующей системы
    static BesLogSystem *getInstance();

public slots:
    //----- Ошибки
    /// Регистрирует сообщение об ошибке подключения к базе данных потока с идентификатором id
    void logDatabaseConnectionFailedMessage(int id);
    //----- Системные сообщения
    /// Регистрирует сообщение о включении сервера
    void logServerStartedMessage();
    /// Регистрирует сообщение о выключении сервера
    void logServerStoppedMessage();
    /// Регистрирует сообщение об успешном подключении рабочего потока с идентификатором id к базе данных
    void logDatabaseConnectionEstablishedMessage(int id);
    //----- Отладочная информация
    /// Регистрирует сообщение о принятии нового клиентского подключения
    void logClientConnectionCreatedMessage();
    /// Регистрирует сообщение о закрытии клиентского подключения
    void logClientConnectionClosedMessage();

    /// Регистрирует сообщение об успешном прохождении пользователем с почтой email аутентификации
    void logClientLoggedInMessage(QString email);
    /// Регистрирует сообщение о неуспешном прохождении пользователем с почтой email аутентификации
    void logClientFailedAuthentication(QString email);
    /// Регистрирует сообщение об отправке кода верификации регистрации на почту email
    void logVerificationCodeWasSent(QString email);
    /// Регистрирует сообщение об успешной регистрации нового пользователя с почтой email
    void logClientWasRegistered(QString email);
    /// Регистрирует сообщение об отправке пользователем с почтой email неправильного кода верификации
    /// регистрации code
    void logClientSentWrongVerificationCode(QString email, QString code);
    /// Регистрирует сообщение об использовании для регистрации занятой почты email
    void logClientUsedOccupiedEmailForRegistration(QString email);
private slots:
    /// Проверяет, закончился ли день. День "заканчивается" в 23.59.
    void checkEndOfDay();

private:
    static BesLogSystem *_instance;
    static mutex _mutex;

protected:
    BesLogSystem(const QString &logFileName, QObject *parent = nullptr);
    ~BesLogSystem();

    /// Настраивает таймеры
    void configureTimers();
    /// Сохраняет журнал сообщений за предыдущий день в отдельном файле с временной пометкой в названии
    void savePreviousLogFile();
    /// Таймер, предназначенный для отслеживания момента, когда надо сохранить журнал сообщений в отдельный файл с временной пометкой
    /// Обычно этот файл должен сохраняться каждый раз во время, описываемое макросом END_OF_DAY_TIME
    QTimer *savePreviousLogFileTimer;

};

#endif // BESLOGSYSTEM_H
