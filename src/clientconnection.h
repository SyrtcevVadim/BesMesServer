#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H
// Автор: Сырцев Вадим Игоревич
#include<QObject>
#include<QTcpSocket>
#include<QTextStream>



#define END_OF_COMMAND "\r\n"
// Команда, отсылаемая серверу для аутентификации пользователя
#define LOGIN_COMMAND "ПРИВЕТ"
#define REGISTRATION_COMMAND "РЕГИСТРАЦИЯ"

enum class CommandType{LogIn, Registration, Unspecified};

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
    /// Отправляет клиенту сообщение
    void sendResponse(QString response);
    void close();
signals:
    /// Отправляется, когда пользователь отправил команду аутентификации и передал
    /// адрес электронной почты и пароль от аккаунта
    void logInCommandSent(QString userName, QString password);
    /// Отправляется, когда пользователь отправил команду регистрации и передал
    /// имя, фамилию, адрес электронной почты и пароль для нового аккаунта
    void registrationCommandSent(QString firstName, QString lastName,
                                 QString email, QString password);
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
    QString receiveIncomingMessage();
    /// Разбивает клиентское сообщение на фрагменты по пробельным
    /// символам. Вовзращает список фрагментов (команда и её аргументы)
    QStringList parseMessage(QString clientMessage);
    /// По названию команды возвращает её тип
    CommandType getCommandType(const QString &commandName);
    /// Обрабатывает команду клиента в зависимости от ее типа.
    /// Вместе с командой передаются её аргументы
    void processCommand(QStringList messageParts);
    /// Сокет, через который клиентское приложение подключено к серверу
    QTcpSocket *socket;
    /// Текстовый поток, связанный с этим сокетом
    QTextStream *stream;
};

#endif // CLIENTCONNECTION_H
