#include <QFile>
#include <QSslKey>
#include <QSslCertificate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "client_connection.h"
#include "config_reader.h"
#include "server_worker.h"
#include "user.h"
#include "chat.h"

QSslConfiguration ClientConnection::sslConfiguration;

void ClientConnection::initSslConfiguration()
{
    sslConfiguration.setProtocol(QSsl::TlsV1_2);
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    ConfigReader &config_reader = ConfigReader::getInstance();

    QString pathToPrivateKey{config_reader.getPathToPrivateKey()};
    QFile *privateKeyFile = new QFile(pathToPrivateKey);
    // Нужно открыть файл, чтобы QSslKey смог прочесть его содержимое
    if(!privateKeyFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "Файл с закрытым ключом не найден";
    }
    QSslKey privateKey(privateKeyFile, QSsl::Rsa,
                       QSsl::Pem, QSsl::PrivateKey,
                       config_reader.getPathPhrase().toUtf8());
    privateKeyFile->close();
    sslConfiguration.setPrivateKey(privateKey);
    // Устанавливает сертификат
    QString pathToCertificate{config_reader.getPathToCertificate()};
    QFile *certificateFile = new QFile(pathToCertificate);
    if(!certificateFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "Файл с сертификатом безопасности не найден";
    }
    QSslCertificate certificate(certificateFile, QSsl::Pem);
    certificateFile->close();
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.addCaCertificate(certificate);


    delete privateKeyFile;
    delete certificateFile;
}

ClientConnection::ClientConnection(qintptr socketDescriptor, QObject *parent) : QObject(parent)
{
    // Создаём сокет защищённого уровня
    socket = new QSslSocket();
    socket->setSocketDescriptor(socketDescriptor);
    // Устанавливаем конфигурацию Ssl для серверного сокета
    socket->setSslConfiguration(sslConfiguration);

    // Первыми инициализируем SSL-рукопожатие
    socket->startServerEncryption();

    connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), SLOT(processIncomingMessage()));
}

ClientConnection::~ClientConnection()
{
    socket->close();
    emit closed();
    delete socket;
    qDebug() << "Клиентское подключение разорвано!";
}

void ClientConnection::close()
{
    socket->close();
}

void ClientConnection::sendResponse(const QJsonObject &response)
{
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

optional<QJsonDocument> ClientConnection::receiveIncomingMessage()
{
    /* Корректно принимает сообщение от клиента целиком.
     * Может случиться так, что сообщение будет разбито на куски, которые будут приняты друг за другом
     * Именно поэтому мы склеивам сообщение из разных кусочков
     * Сообщения передаются в кодировке UTF-16
     */
    static QString clientMessage="";
    clientMessage += QString::fromUtf8(socket->readAll());


    // Пытаемся собрать объект Json-документа
    QJsonDocument incomingMessage = QJsonDocument::fromJson(clientMessage.toUtf8());
    if(incomingMessage.isNull())
    {
        // Пустая строка означает, что сообщение принято не до конца
        return std::nullopt;
    }
    // Очищаем прочитанное сообщение, чтобы быть готовым прочесть новое
    clientMessage.clear();
    clientMessage.reserve(2048);
    return incomingMessage;
}

void ClientConnection::processQuery(const QJsonDocument &queryDocument)
{
    QString query = queryDocument[QUERY_TYPE_KEY].toString();

    ServerWorker *worker = static_cast<ServerWorker*>(parent());
    QJsonObject response{
        {QUERY_TYPE_KEY, query},
        {QUERY_RESPONSE_KEY, 0}
    };

    if (query == LOGIN_QUERY) {
        if (worker->verifyLogIn(queryDocument[USER_EMAIL_KEY].toString(),
                                  queryDocument[USER_PASSWORD_KEY].toString()))
        {
            response[QUERY_RESPONSE_KEY] = 0;
            // Запоминаем идентификатор пользователя
            user.userId = worker->getUserId(queryDocument[USER_EMAIL_KEY].toString());
            response[USER_ID_KEY] = user.userId;
        }
        else
        {
            response[QUERY_RESPONSE_KEY] = 1;
        }

    }
    else if (query == REGISTRATION_QUERY) {
        if (worker->registerNewUser(queryDocument[USER_FIRST_NAME_KEY].toString(),
                                      queryDocument[USER_LAST_NAME_KEY].toString(),
                                      queryDocument[USER_EMAIL_KEY].toString(),
                                      queryDocument[USER_PASSWORD_KEY].toString()))
        {
            response[QUERY_RESPONSE_KEY] = 0;
        }
        else
        {
            response[QUERY_RESPONSE_KEY] = 1;
        }
    }
    else if (query == GET_USERS_LIST_QUERY) {
        QVector<User> users = worker->getListOfUsers();
        QJsonArray array_of_users;
        QJsonObject userObject;

        for (User &user: users)
        {
            userObject[USER_ID_KEY] = user.userId;
            userObject[USER_FIRST_NAME_KEY] = user.firstName;
            userObject[USER_LAST_NAME_KEY] = user.lastName;
            userObject[USER_EMAIL_KEY] = user.email;
            array_of_users.append(userObject);
        }
        response.insert(USERS_KEY, array_of_users);
    }
    else if (query == GET_CHATS_LIST_QUERY) {
        QVector<Chat> chats = worker->getListOfChats(user.userId);
        QJsonArray array_of_chats;
        QJsonObject chatObject;

        for (Chat &chat: chats)
        {
            chatObject[CHAT_ID_KEY] = chat.chatId;
            chatObject[CHAT_TITLE_KEY] = chat.chatTitle;
            array_of_chats.append(chatObject);
        }
        response.insert(CHATS_KEY, array_of_chats);
    }
    else if (query == SEND_MESSAGE_QUERY) {
        qint64 sending_time = worker->sendMessage(static_cast<qint64>(queryDocument[CHAT_ID_KEY].toDouble()),
                            queryDocument[MESSAGE_BODY_KEY].toString(),
                            static_cast<qint64>(queryDocument[SENDER_ID_KEY].toDouble()));
        response[MESSAGE_SENDING_TIME_KEY] = sending_time;
    }
    else if (query == CREATE_CHAT_QUERY) {
        qint64 chatId = worker->createNewChat(user.userId,
                    queryDocument[CHAT_NAME_KEY].toString());
        response[CHAT_ID_KEY] = chatId;
    }
    else if (query == DELETE_CHAT_QUERY) {
        worker->deleteChat(static_cast<qint64>(queryDocument[CHAT_ID_KEY].toDouble()));
    }
    else if (query == INVITE_TO_CHAT_QUERY) {
        worker->inviteToChat(static_cast<qint64>(queryDocument[CHAT_ID_KEY].toDouble()),
                             static_cast<qint64>(queryDocument[USER_ID_KEY].toDouble()));
    }
    else if (query == KICK_FROM_CHAT_QUERY) {
        worker->kickFromChat(static_cast<qint64>(queryDocument[CHAT_ID_KEY].toDouble()),
                             static_cast<qint64>(queryDocument[USER_ID_KEY].toDouble()));
    }
    else if (query == SYNCHRONIZATION_QUERY) {
        QVector<Chat> chatsWithUnreadMessages = worker->synchronize(user.userId,
                                                                    static_cast<qint64>(queryDocument[MESSAGE_SENDING_TIME_KEY].toDouble()));
        QJsonArray chatsArray;
        QJsonObject chatObject;
        for (Chat &chat: chatsWithUnreadMessages)
        {
            chatObject[CHAT_ID_KEY] = chat.chatId;
            QJsonArray messagesArray;
            QJsonObject messageObject;
            for (Message &message: chat.unreadMessages)
            {
                messageObject[MESSAGE_BODY_KEY] = message.body;
                messageObject[SENDER_ID_KEY] = message.senderId;
                messageObject[SENDING_MESSAGE_TIMESTAMP] = message.sendingTimestamp;
                messagesArray.append(messageObject);
            }

            chatObject[MESSAGES_KEY] = messagesArray;
            chatsArray.append(chatObject);
        }
        response[CHATS_KEY] =chatsArray;
        qDebug() << response;
    }
    else {
        return;
    }
    sendResponse(response);
}

void ClientConnection::processIncomingMessage()
{
    optional<QJsonDocument> clientMessage = receiveIncomingMessage();
    if(!clientMessage) {
        return;
    }
    qDebug() << "Пользователь отправил сообщение: "<< *clientMessage;
    processQuery(*clientMessage);
}
