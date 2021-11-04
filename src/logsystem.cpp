#include "logsystem.h"
#include <QDebug>
#include <QDateTime>

LogSystem::LogSystem(const QString &logFileName, QObject *parent) : QThread(parent)
{
    latestLogFile = new QFile(logFileName);
    // В начале каждой сессии файл логов перезаписывается
    if(!latestLogFile->open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        qDebug() << "!!!Файл логов не открыт";
    }
    logStream = new QTextStream(latestLogFile);
}
LogSystem::~LogSystem()
{
    latestLogFile->close();
}

void LogSystem::run()
{
    exec();
}

void LogSystem::logToFile(QString message)
{
    QDateTime now = QDateTime::currentDateTime();
    QString outMessage = QString("%1:%2\r\n").arg(now.toString(), message);
    *logStream << outMessage;
    logStream->flush();
    qDebug() << outMessage;
    emit messageLogged(outMessage);

}

void LogSystem::close()
{
    latestLogFile->close();
}
