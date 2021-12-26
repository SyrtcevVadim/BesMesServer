#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include "beslogsystem.h"

BesLogSystem *BesLogSystem::_instance{nullptr};
mutex BesLogSystem::_mutex;

BesLogSystem::BesLogSystem(QObject *parent):
    LogSystem(parent)
{
    // Настраиваем таймер для сохранения логов за предыдущий день
    configureTimers();
}

BesLogSystem::~BesLogSystem()
{
    savePreviousLogFileTimer->stop();
    delete savePreviousLogFileTimer;
}

BesLogSystem* BesLogSystem::getInstance()
{
    // Когда какой-либо поток пытается получить объект логгирующей системы, остальные потоки не смогут сделать этого
    lock_guard<mutex> lock(_mutex);
    if(_instance == nullptr)
    {
        _instance = new BesLogSystem();
    }
    return _instance;
}


void BesLogSystem::configureTimers()
{
    savePreviousLogFileTimer = new QTimer();
    savePreviousLogFileTimer->setInterval(CHECK_END_OF_DAY_INTERVAL);
    savePreviousLogFileTimer->start();
    connect(savePreviousLogFileTimer, SIGNAL(timeout()), SLOT(checkEndOfDay()));
}

void BesLogSystem::checkEndOfDay()
{
    // Сравниваем именно строки, чтобы не учитывать миллисекунды в текущем времени
    if(QTime::currentTime().toString("hh:mm:ss")==END_OF_DAY_TIME.toString("hh:mm:ss"))
    {
        savePreviousLogFiles();
    }
}

void BesLogSystem::savePreviousLogFiles()
{
    qDebug() << "Сохраняем старые журналы сообщений";
    // Закрываем текущие журналы сообщений
    close();
    QString timeStamp = QDateTime::currentDateTime().toString("dd-MM-yyyy");
    // Создаём папку для журналов сообщений предыдущего дня
    QDir previousDayLogsDir;
    previousDayLogsDir.cd(LOG_DIR_NAME);
    previousDayLogsDir.mkdir(timeStamp);

    // Перемещаем файлы в папку журналы сообщений за прошедший день

    qDebug() << systemLog->copy(QString("%1/%2/%3")
                    .arg(LOG_DIR_NAME, timeStamp, QString("system.txt")));
    debugLog->copy(QString("%1/%2/%3")
                   .arg(LOG_DIR_NAME, timeStamp, QString("debug.txt")));
    errorLog->copy(QString("%1/%2/%3")
                   .arg(LOG_DIR_NAME, timeStamp, QString("error.txt")));
    // Перезаписываем журналы сообщений
    systemLog->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);
    debugLog->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);
    errorLog->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);
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
