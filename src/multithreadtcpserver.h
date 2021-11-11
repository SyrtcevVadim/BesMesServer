#ifndef MULTITHREADTCPSERVER_H
#define MULTITHREADTCPSERVER_H
// Автор: Сырцев Вадим Игоревич
#include <QTcpServer>
#include <QVector>

#include "serverworker.h"
#include "serverstatisticscounter.h"
#include "logsystem.h"
#include "configfileeditor.h"

/**
 * Описывает многопоточный TCP-сервер, способный принимать входящие подключения
 * клиентских приложений, отвечать на пользовательские запросы
 */
class MultithreadTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MultithreadTcpServer(QHostAddress serverIPAddress,
                         qint16 serverPort,
                         QObject *parent = nullptr);
    ~MultithreadTcpServer();
signals:
    /// Сигнал, высылаемый, когда сервер перестаёт прослушивать входящие соединения
    void stopped();
    /// Сигнал, высылаемый после открытия нового соединения с клиентским приложением
    void clientConnectionOpenned();
    /// Сигнал, высылаемый после разрыва соединения с клиентским приложением
    void clientConnectionClosed();
    /// Сигнал, высылаемый после открытия или разрыва клиентского соединения
    /// activeConnectionsCounter - количество активных соединений
    void activeConnectionsCounterChanged(unsigned long long activeConnectionsCounter);
    /// Сигнал о намерении зарегистрировать сообщение в файле
    void logMessage(QString message);
public slots:
    /// Запускает работу сервера: прослушивание входящих соединений
    void start();
    /// Останавливает работу сервера
    void stop();

protected:
    /// Вызывается сервером каждый раз, когда имеется входящее соединение
    void incomingConnection(qintptr socketDescriptor);
private slots:
    // TODO
private:
    /// Создаёт оптимальное количество рабочих потоков, по которым будет распределена
    /// нагрузка входящих соединений, основываясь на значении possibleThreadNumber
    void initWorkers();
    /// Освобождает ресурсы, выделенные для рабочих, обрабатывающих входящие
    /// сообщения
    void removeWorkers();

    /// Связывает сигналы и слоты, необходимые для ведения статистического учёта объектом-счётчиком
    void configureStatisticsCounter();
    /// Связывает сигналы и слоты, необходимые для корректной работы системы регистрации сообщений
    void configureLogSystem();
private:
    /// Хранит количество потоков, которые физически(и в теории) могут выполняться независимо
    /// на разных ядрах процессора, т.е. оптимальное количество потоков. Ровно столько серверных рабочих
    /// будет создано в serverWorkers
    static size_t workerThreadsNumber;
    /// Стандартное количество потоков, которые создаст сервер в случае, если он не сможет определить оптимальное
    /// количество потоков на рабочем устройстве
    static const size_t DEFAULT_THREAD_NUMBER=8;

    /// Список рабочих, обрабатывающих в отдельных потоках
    /// пользовательские соединения
    QVector<ServerWorker*> serverWorkers;
    /// Объект, подсчитывающий статистику сервера во время его работы
    ServerStatisticsCounter *statisticsCounter;
    /// Логгирующая система, записывающая все действия, выполняемые сервером,
    /// в отдельный файл
    LogSystem *logSystem;
    /// Редактирует настройки в файле конфигурации и позволяет их получать
    ConfigFileEditor *configEditor;


    /// IP-адрес устройства, на котором запущен сервер
    QHostAddress serverIPAddress;
    /// Порт, прослушиваемый сервером
    qint16 serverPort;
};

#endif // MULTITHREADTCPSERVER_H
