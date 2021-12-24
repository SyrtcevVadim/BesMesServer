#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H
// Автор: Сырцев Вадим Игоревич
#include <QThread>
#include <QFile>
#include <QTextStream>

// Названия файлов с логами
#define SYSTEM_LOG_FILE_NAME QString("system.txt")
#define DEBUG_LOG_FILE_NAME QString("debug.txt")
#define ERROR_LOG_FILE_NAME QString("error.txt")

#define STANDART_LOG_DIR_NAME QString("logs")

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
    LogSystem(QObject *parent = nullptr);
    ~LogSystem();

    /// Создаёт директорию для файлов логов. TODO перенести в ProjectStructureDefender
    void static createLogsDirectory();

public slots:
    /// Закрывает журнал сообщений. После этого сообщения не будут записываться в файлы
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

    /// Лог системных сообщений
    QFile *systemLog;
    /// Текстовый поток, связанный с файлов системных сообщений
    QTextStream *systemStream;

    /// Лог отладочных сообщений
    QFile *debugLog;
    /// Текстовый поток, связанный с файлом отладочных сообщений
    QTextStream *debugStream;

    /// Лог сообщений об ошибках
    QFile *errorLog;
    /// Текстовый поток, связанный с файлом сообщений об ошибках
    QTextStream *errorStream;
};

#endif // LOGSYSTEM_H
