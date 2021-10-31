// Автор: Сырцев Вадим Игоревич
#ifndef MULTITHREADTCPSERVER_H
#define MULTITHREADTCPSERVER_H

#include <QTcpServer>
/**
 * Описывает многопоточный TCP-сервер, способный принимать входящие подключения
 * клиентских приложений, отвечать на пользовательские запросы
 */
class MultithreadTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MultithreadTcpServer(QWidget *parent = nullptr);
    ~MultithreadTcpServer();

    /// Хранит количество потоков, которые физически(и в теории) могут выполняться независимо
    /// на разных ядрах процессора
    static size_t possibleThreadNumber;
};

#endif // MULTITHREADTCPSERVER_H
