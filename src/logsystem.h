#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H
// Автор: Сырцев Вадим Игоревич
#include <QThread>
#include <QFile>
#include <QTextStream>


#define STANDART_LOG_DIR_NAME QString("logs")
#define STANDART_LOG_FILE_NAME QString("latest.txt")

#define ERROR_MESSAGE_MARKER "!"
#define DEBUG_MESSAGE_MARKER "#"
#define SYSTEM_MESSAGE_MARKER "^"

/// Каждое сообщение в логгирующей системе имеет тип. Эти типы описываются данным
/// перечислением:
/// Error - ошибка, возникшая во время работы приложения
/// Debug - отладочная информация для разработчика
/// System - системная информация(например, информация о периодах работы приложения)
enum class MessageType{Error, Debug, System};

/// Объект логгирующей системы работает в отдельном потоке и регистрирует деятельность
/// сервера в файл. Каждый день содержимое файла конфигурации архивируется и сохраняется в
/// файловой системой с временной пометкой в названии
class LogSystem : public QThread
{
    Q_OBJECT

public:
    LogSystem(const QString &logFileName, QObject *parent = nullptr);
    ~LogSystem();

    /// Создаёт директорию для файлов логов. TODO перенести в ProjectStructureDefender
    void static createLogsDirectory();
public slots:
    /// Закрывает файл регистрации сообщений
    void close();

signals:
    /// Сигнал, высылаемый после регистрации сообщения
    void messageLogged(QString message);

protected slots:
    /// Регистрирует сообщение в файле.
    /// type - тип регистрируемого сообщения
    /// message - строка сообщения
    void logToFile(MessageType type, QString message);

protected:
    void run();

    /// Название текущего журнала сообщений
    QString logFileName;
    /// Файл, в который будут записываться логи текущего сеанса
    QFile *logFile;
    /// Текстовый поток, связанный с файлом логов
    QTextStream *logStream;

};

#endif // LOGSYSTEM_H
