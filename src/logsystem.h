#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H
// Автор: Сырцев Вадим Игоревич
#include <QThread>
#include <QFile>
#include <QTextStream>

#define LOG_FILE_NAME "latest.txt"

///
/// Объект логгирующей системы крутится в отдельном потоке и регистрирует деятельность
/// сервера в выходном файле
/// TODO В случае возникновения чрезвычайной ситуации, создаётся отдельный файл,
/// в который копируется содержимое последнего файла логов.
class LogSystem : public QThread
{
    Q_OBJECT
public:
    LogSystem(const QString &logFileName, QObject *parent = nullptr);
    ~LogSystem();
public slots:
    /// Регистрирует сообщение message в файле
    void logToFile(QString message);
    /// Закрывает файл регистрации сообщений
    void close();
signals:
    /// Сигнал, высылаемый после регистрации сообщения
    void messageLogged(QString message);
protected:
    void run();
private:
    /// Файл, в который будут записываться логи текущего сеанса
    QFile *latestLogFile;
    /// Текстовый поток, связанный с файлом логов
    QTextStream *logStream;
};

#endif // LOGSYSTEM_H
