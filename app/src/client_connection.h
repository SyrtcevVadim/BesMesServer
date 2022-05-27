#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H
// Автор: Сырцев Вадим Игоревич
#include<QObject>
#include<QSslSocket>
#include<QSslConfiguration>
#include<QTextStream>
#include<QJsonDocument>
#include<optional>

#include"bes_protocol.h"
#include"user.h"

using namespace std;

enum class Query{
    LogIn,
    Registration,
    GetUsersList,
    GetChatsList,
    GetUnreadMessages,
    SendMessage,
    CreateChat,
    DeleteChat,
    InviteToChat,
    KickFromChat,
    RefreshChat,
    Synchronization,
    Unspecified};

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
    explicit ClientConnection(qintptr socketDescriptor, QObject *parent = nullptr);
    ~ClientConnection();

public slots:
    /// Отправляет клиенту сообщение.
    /// К сообщению автоматически добавляется символ конца сообщения.
    void sendResponse(const QJsonDocument &response);
    /// Закрывает клиентское соединение
    void close();
    /// Устанавливает параметры для корректной работы ssl-соединения. Следует вызвать эту функцию
    /// в начале работы программы
    static void initSslConfiguration();
signals:
    /// Отправляется после разрыва клиентского соединения с сервером
    void closed();
private slots:
    /// Обрабатывает входящую команду пользователя. В зависимости
    /// от типа команды отсылается соответствующий сигнал с данными
    void processIncomingMessage();
private:
    /// Отвечает за то, чтобы принять входящее клиентское сообщение целиком, т.е.
    /// оно должно заканчиваться последовательностью CRLF. Пока сообщение не принято целиком,
    /// возвращается пустая строка. Как только сообщение полностью принято, возвращает
    /// принятое сообщение
    optional<QJsonDocument> receiveIncomingMessage();
    /// Обрабатывает команду клиента в зависимости от ее типа.
    void processQuery(const QJsonDocument &queryDocument);
    /// Сокет, через который клиентское приложение подключено к серверу
    QSslSocket *socket;
    /// Структура, хранящая информацию о пользователе
    User user;
    /// Хранит параметры, необходимые для работы ssl-сокетов
    static QSslConfiguration sslConfiguration;
    /// Объекты класса ClientConnection "живут" в объектах класса ServerWorker. Так что будет разумным
    /// дать объектам этого класса доступ к внутренним переменным
    friend class ServerWorker;
};

#endif // CLIENT_CONNECTION_H
