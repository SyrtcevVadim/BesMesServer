// Автор: Сырцев Вадим Игоревич
#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include "configfileeditor.h"
#include <QThread>

/**
 * Описывает поток сервера, в котором могут обрабатываться пользовательские подключения
 */
class ServerWorker : public QThread
{
    Q_OBJECT
public:
    ServerWorker(QObject *parent = nullptr);
    /// Добавляет в данный серверный поток обработки новое клиентское
    /// соединение
    void addClientConnection(qintptr socketDescriptor);
    /// Возвращает количество клиентских подключений, обрабатываем текущим рабочим потоком
    unsigned long long getHandlingConnectionsCounter();

signals:
    /// Сигнал, отправляемый всем подключенным к данному потоку соединениям, уведомляющий о том, что
    /// нужно разорвать соединение
    void stopWorker();
    /// Сигнал, высылаемый, когда клиентское соединение, обрабатываемое
    /// в данном потоке, разрывается
    void clientConnectionClosed();
protected:
    void run();
private slots:
    /// Обрабатывает сообщение пользователя об аутентификации
    void processHelloMessage(QString userName, QString password);
    /// Уменьшает счётчик клиентских соединений, обрабатываемых текущим потоком
    void decreaseHandlingConnectionsCounter();
private:
    /// Счётчик созданных объектов
    static unsigned int createdObjectCounter;
    /// Номер потока, работающего в рамках серверного приложения
    unsigned int id;
    /// Счётчик соединений, обрабатываемых данным рабочим потоком
    unsigned long long handlingConnectionsCounter;

};

#endif // SERVERWORKER_H
