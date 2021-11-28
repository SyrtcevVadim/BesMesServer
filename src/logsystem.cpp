#include "logsystem.h"
#include <QDebug>
#include <QDateTime>
#include <QDir>

LogSystem::LogSystem(const QString &logFileName, QObject *parent) : QThread(parent)
{
    logFile = new QFile(QDir::currentPath()+"/"+STANDART_LOG_DIR_NAME+"/"+logFileName);
    // В начале каждой сессии файл логов перезаписывается
    if(!logFile->open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        qDebug() << "!!!Файл логов не открыт";
    }
    logStream = new QTextStream(logFile);
}
LogSystem::~LogSystem()
{
    logFile->close();
}

void LogSystem::createLogsDirectory()
{
    QDir currentDirectory;
    currentDirectory.mkdir(STANDART_LOG_DIR_NAME);
}

void LogSystem::run()
{
    exec();
}

void LogSystem::logToFile(MessageType type, QString message)
{
    QDateTime now = QDateTime::currentDateTime();
    // Выбираем соответствующий маркер сообщения
    QString messageMarker="";
    switch(type)
    {
        case MessageType::Error:
        {
            messageMarker = ERROR_MESSAGE_MARKER;
            break;
        }
        case MessageType::Debug:
        {
            messageMarker = DEBUG_MESSAGE_MARKER;
            break;
        }
        case MessageType::System:
        {
            messageMarker = SYSTEM_MESSAGE_MARKER;
            break;
        }
    }

    QString outMessage = QString("%1 %2 %3\n").arg(now.toString("hh:mm:ss"),messageMarker, message);
    *logStream << outMessage;
    logStream->flush();
    emit messageLogged(outMessage);
}

void LogSystem::close()
{
    logFile->close();
}
