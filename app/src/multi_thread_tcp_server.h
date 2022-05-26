#ifndef MULTI_THREAD_TCP_SERVER_H
#define MULTI_THREAD_TCP_SERVER_H
// Автор: Сырцев Вадим Игоревич
#include <QTcpServer>
#include <QVector>
#include <QTimer>

#include "time_counter.h"
#include "server_worker.h"
#include "server_statistics_counter.h"
#include "config_reader.h"

// Интервал обновления счётчика времени работы приложения
#define WORKING_TIME_COUNTER_UPDATE_TIME 1000

/**
 * Описывает многопоточный TCP-сервер, способный принимать входящие подключения
 * клиентских приложений, отвечать на пользовательские запросы
 */
class MultithreadTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MultithreadTcpServer(QHostAddress serverIPAddress,
                         QObject *parent = nullptr);
    ~MultithreadTcpServer();
signals:
    /// Сигнал, высылаемый, когда сервер начинает прослушивать входящие соединения
    void started();
    /// Сигнал, высылаемый, когда сервер перестаёт прослушивать входящие соединения
    void stopped();
    /// Сигнал, высылаемый после открытия нового соединения с клиентским приложением
    void clientConnectionOpenned();
    /// Сигнал, высылаемый после разрыва соединения с клиентским приложением
    void clientConnectionClosed();
    /// Сигнал, высылаемый после открытия или разрыва клиентского соединения
    /// activeConnectionsCounter - количество активных соединений
    void activeConnectionsCounterChanged(unsigned long long activeConnectionsCounter);
    /// Отправляется каждый раз, когда время работы сервера в текущей
    /// сессии обновляется
    void workingTimeUpdated(QString time);
public slots:
    /// Запускает работу сервера: прослушивание входящих соединений
    void start();
    /// Останавливает работу сервера
    void stop();

protected:
    /// Вызывается сервером каждый раз, когда имеется входящее соединение
    void incomingConnection(qintptr socketDescriptor);
private slots:
    /// Обновляет счётчик времени работы сервера в текущей сессии
    void updateWorkingTimeCounter();
private:
    /// Создаёт оптимальное количество рабочих потоков, по которым будет распределена
    /// нагрузка входящих соединений, основываясь на значении possibleThreadNumber
    void initWorkers();
    /// Освобождает ресурсы, выделенные для рабочих, обрабатывающих входящие
    /// сообщения
    void removeWorkers();

    /// Настраивает объект-счётчик
    void configureStatisticsCounter();
    /// Настраивает таймеры сервера:
    /// Таймер счётчика времени работы сервера
    void configureTimers();
private:
    /// Хранит количество потоков, которые физически(и в теории) могут выполняться независимо
    /// на разных ядрах процессора, т.е. оптимальное количество потоков. Ровно столько серверных рабочих
    /// будет создано в serverWorkers
    static int workerThreadsNumber;
    /// Стандартное количество потоков, которые создаст сервер в случае, если он не сможет определить оптимальное
    /// количество потоков на рабочем устройстве
    static const int DEFAULT_THREAD_NUMBER = 4;

    /// Список рабочих, обрабатывающих в отдельных потоках
    /// пользовательские соединения
    QVector<ServerWorker*> serverWorkers;
    /// Объект, подсчитывающий статистику сервера во время его работы
    ServerStatisticsCounter *statisticsCounter;
    /// IP-адрес устройства, на котором запущен сервер
    QHostAddress serverIPAddress;
    /// Таймер, оповещающий UI, что следует обновить время работы
    /// серверного приложения в текущей сессии (после запуска)
    QTimer currentSessionWorkingTimeTimer;
    /// Время работы приложения в текущей сессии
    TimeCounter currentSessionWorkingTime;
};

#endif // MULTI_THREAD_TCP_SERVER_H
