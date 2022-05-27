#include <QFile>
#include <QSslKey>
#include <QSslCertificate>
#include <QJsonDocument>
#include <QJsonObject>
#include "client_connection.h"
#include "config_reader.h"

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

void ClientConnection::sendResponse(const QJsonDocument &response)
{
    socket->write(response.toJson());
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
    QString query = queryDocument[QUERY_TITLE].toString();
    if (query == LOGIN_QUERY) {
        QJsonObject response{
            {"тип_запроса","ЛОГИН"},
            {"код_ответа", 0}};
        sendResponse(QJsonDocument(response));
    }
    else if (query == REGISTRATION_QUERY) {
        QJsonObject response{
            {"тип_запроса","РЕГИСТРАЦИЯ"},
            {"код_ответа", 0}};
        sendResponse(QJsonDocument(response));
    }
    else if (query == GET_USERS_LIST_QUERY) {

    }
    else if (query == GET_CHATS_LIST_QUERY) {

    }
    else if (query == SEND_MESSAGE_QUERY) {

    }
    else if (query == CREATE_CHAT_QUERY) {

    }
    else if (query == DELETE_CHAT_QUERY) {

    }
    else if (query == INVITE_TO_CHAT_QUERY) {

    }
    else if (query == KICK_FROM_CHAT_QUERY) {

    }
    else if (query == REFRESH_CHAT_QUERY) {

    }
    else if (query == SYNCHRONIZATION_QUERY) {

    }
    else {

    }
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
