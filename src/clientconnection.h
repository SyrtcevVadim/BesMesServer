#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include<QObject>
#include<QTcpSocket>
#include<QTextStream>

// Команда, отсылаемая серверу для аутентификации пользователя
#define HELLO_COMMAND "HELLO"

/**
 * Описываем входщящее клиентское соединение, которое будет обрабатываться одним из
 * серверных рабочих потоков. Инкапсулирует запросы клиента методами
 */
class ClientConnection : public QObject
{
    Q_OBJECT
public:
    /// Создаёт объект входящего клиентского соединения на основе дескриптора сокета,
    /// который принимается сервером при подключении нового клиента
    explicit ClientConnection(qintptr socketDescriptor,QObject *parent = nullptr);
    ~ClientConnection();
public slots:
    void receiveServerResponse(QString response);
    void close();
signals:
    /// Отправляется, когда пользователь отправил команду ПРИВЕТ и передал
    /// имя пользователя и пароль от аккаунта
    void helloSaid(QString userName, QString password);
    /// Отправляется после разрыва клиентского соединения с сервером
    void closed();
private slots:
    /// Обрабатывает входящую команду пользователя. В зависимости
    /// от типа команды отсылается соответствующий сигнал с данными
    void processIncomingMessage();
private:
    /// Сокет, через который клиентское приложение подключено к серверу
    QTcpSocket *socket;
    /// Текстовый поток, связанный с этим сокетом
    QTextStream *stream;
};

#endif // CLIENTCONNECTION_H
