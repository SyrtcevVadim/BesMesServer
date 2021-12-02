#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

#include "beslogsystem.h"

BesLogSystem::BesLogSystem(const QString &logFileName, QObject *parent):
    LogSystem(logFileName, parent)
{
    configureTimers();
    connect(savePreviousLogFileTimer, SIGNAL(timeout()), SLOT(checkEndOfDay()));

}

BesLogSystem::~BesLogSystem()
{
    savePreviousLogFileTimer->stop();
    delete savePreviousLogFileTimer;
}

void BesLogSystem::configureTimers()
{
    savePreviousLogFileTimer = new QTimer();
    savePreviousLogFileTimer->setInterval(CHECK_END_OF_DAY_INTERVAL);
    savePreviousLogFileTimer->start();
}

void BesLogSystem::checkEndOfDay()
{
    // Сравниваем именно строки, чтобы не учитывать миллисекунды в текущем времени
    if(QTime::currentTime().toString("hh:mm:ss")==END_OF_DAY_TIME.toString("hh:mm:ss"))
    {
        savePreviousLogFile();
    }
}

void BesLogSystem::savePreviousLogFile()
{
    // Закрываем текущий журнал сообщений
    logFile->close();
    // Переименовываем текущий журнал сообщений в файл с временной пометкой
    logFile->rename(STANDART_LOG_DIR_NAME+"/"+QDateTime::currentDateTime().toString("dd-MM-yyyy")+".txt");

    logFile->setFileName(STANDART_LOG_DIR_NAME+"/"+STANDART_LOG_FILE_NAME);
    logFile->open(QIODevice::Truncate | QIODevice::WriteOnly);
}

void BesLogSystem::logServerStartedMessage()
{
    logToFile(MessageType::System, "сервер включён");
}

void BesLogSystem::logServerStoppedMessage()
{
    logToFile(MessageType::System, "сервер выключен");
}

void BesLogSystem::logClientConnectionCreatedMessage()
{
    // TODO добавить идентификатор пользовательского соединения
    logToFile(MessageType::Debug, "создано новое клиентское подключение");
}

void BesLogSystem::logClientConnectionClosedMessage()
{
    // TODO добавить идентификатор пользовательского соединения
    logToFile(MessageType::Debug, "разорвано клиентское соединение");
}


void BesLogSystem::logDatabaseConnectionFailedMessage(int id)
{
    logToFile(MessageType::Error, QString("поток %1 не смог установить соединение с базой данных")
              .arg(id));
}

void BesLogSystem::logDatabaseConnectionEstablishedMessage(int id)
{
    logToFile(MessageType::System, QString("поток %1 смог установить соединение с базой данных")
              .arg(id));
}

void BesLogSystem::logClientLoggedInMessage(QString email)
{
    logToFile(MessageType::Debug, QString("пользователь \"%1\" прошёл аутентификацию")
              .arg(email));
}

void BesLogSystem::logClientFailedAuthentication(QString email)
{
    logToFile(MessageType::Debug, QString("пользователь \"%1\" не прошёл аутентификацю")
              .arg(email));
}

void BesLogSystem::logVerificationCodeWasSent(QString email)
{
    logToFile(MessageType::Debug, QString("пользователю с почтой \"%1\" отправлен код верификации регистрации")
              .arg(email));
}

void BesLogSystem::logClientWasRegistered(QString email)
{
    logToFile(MessageType::Debug, QString("зарегистрирован новый пользователь \"%1\"")
              .arg(email));
}

void BesLogSystem::logClientSentWrongVerificationCode(QString email, QString code)
{
    logToFile(MessageType::Debug, QString("пользователь \"%1\" отправил неправильный код верификации %2")
              .arg(email, code));
}

void BesLogSystem::logClientUsedOccupiedEmailForRegistration(QString email)
{
    logToFile(MessageType::Debug, QString("для регистрации была использована занятая почта \"%1\"")
              .arg(email));
}