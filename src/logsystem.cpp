#include "logsystem.h"
#include <QDebug>
#include <QDateTime>
#include <QDir>

LogSystem::LogSystem(QObject *parent) : QThread(parent)
{
    systemLog = new QFile(QString("%1/%2")
                          .arg(LOG_DIR_NAME, SYSTEM_LOG_FILE_NAME));
    systemStream = new QTextStream(systemLog);

    debugLog = new QFile(QString("%1/%2")
                         .arg(LOG_DIR_NAME, DEBUG_LOG_FILE_NAME));
    debugStream = new QTextStream(debugLog);

    errorLog = new QFile(QString("%1/%2")
                         .arg(LOG_DIR_NAME, ERROR_LOG_FILE_NAME));
    errorStream = new QTextStream(errorLog);


    // Создаём файлы
    if(!systemLog->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл системных логов!";
    }
    if(!debugLog->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл логов отладочных сообщений";
    }
    if(!errorLog->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл сообщений об ошибках";
    }
}

LogSystem::~LogSystem()
{
    close();
}

void LogSystem::run()
{
    exec();
}

void LogSystem::logToFile(MessageType type, QString message)
{
    QDateTime now = QDateTime::currentDateTime();
    // В зависимости от типа сообщение попадает в свой файл: с системными сообщениями, с отладочными сообщения,
    // с сообщениями об ошибках
    QString outMessage = QString("%1 %2\n").arg(now.toString("hh:mm:ss"),message);
    switch(type)
    {
        case MessageType::System:
        {
            *systemStream << outMessage;
            systemStream->flush();
            break;
        }
        case MessageType::Debug:
        {
            *debugStream << outMessage;
            debugStream->flush();
            break;
        }
        case MessageType::Error:
        {
            *errorStream << outMessage;
            errorStream->flush();
            break;
        }
    }
    qDebug() << outMessage;
    emit messageLogged(outMessage);
}

void LogSystem::close()
{
    // Закрываем все файлы логов
    systemLog->close();
    debugLog->close();
    errorLog->close();
}
