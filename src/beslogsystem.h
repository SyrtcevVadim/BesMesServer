#ifndef BESLOGSYSTEM_H
#define BESLOGSYSTEM_H

#include "logsystem.h"


class BesLogSystem : public LogSystem
{
    Q_OBJECT
public:
    BesLogSystem(const QString &logFileName, QObject *parent = nullptr);
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
};

#endif // BESLOGSYSTEM_H
