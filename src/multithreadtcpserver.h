// Автор: Сырцев Вадим Игоревич
#ifndef MULTITHREADTCPSERVER_H
#define MULTITHREADTCPSERVER_H

#include <QTcpServer>
#include <QVector>

#include "serverworker.h"

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
    void serverStopped();
public slots:
    /// Запускает работу сервера: прослушивание входящих соединений
    void start();
    /// Останавливает работу сервера
    void stop();
protected:
    /// Вызывается сервером каждый раз, когда имеется входящее соединение
    void incomingConnection(qintptr socketDescriptor);
private:
    /// Создаёт оптимальное количество рабочих, по которым будет распределена
    /// нагрузка входящих соединений, основываясь на значении possibleThreadNumber
    void initWorkers();
    /// Освобождает ресурсы, выделенные для рабочих, обрабатывающих входящие
    /// сообщения
    void removeWorkers();
private:
    /// Хранит количество потоков, которые физически(и в теории) могут выполняться независимо
    /// на разных ядрах процессора, т.е. оптимальное количество потоков. Ровно столько серверных рабочих
    /// будет создано в serverWorkers
    static size_t threadsNumber;
    /// Стандартное количество потоков, которые создаст сервер в случае, если он не сможет определить оптимальное
    /// количество потоков на рабочем устройстве
    static const size_t DEFAULT_THREAD_NUMBER=8;

    /// Счётчик входящих соединений. Используется для равномерной балансировки нагрузки на сервер между
    /// серверными рабочими по алгоритму Round Robin (кусочек каждому)
    unsigned int incomingConnectionsCounter;
    /// Список рабочих, обрабатывающих в отдельных потоках
    /// пользовательские соединения
    QVector<ServerWorker*> serverWorkers;

    /// IP-адрес устройства, на котором запущен сервер
    QHostAddress serverIPAddress;
    /// Порт, прослушиваемый сервером
    qint16 serverPort;
};

#endif // MULTITHREADTCPSERVER_H
