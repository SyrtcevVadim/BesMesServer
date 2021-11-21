#include<QFile>
#include "clientconnection.h"

#define KEY_FILE_NAME "besmes.key"
#define CERTIFICATE_FILE_NAME "besmes.crt"
#define PASS_PHRASE "besmes"

ClientConnection::ClientConnection(qintptr socketDescriptor, QObject *parent) : QObject(parent)
{
    // Обнуляем все флаги статуса
    statusFlags=0;
    // Создаём сокет защищённого уровня
    socket = new QSslSocket();
    socket->setSocketDescriptor(socketDescriptor);
    socket->setProtocol(QSsl::TlsV1_3OrLater);

    // Устанавливаем открытый ключ и закрытый
    socket->setLocalCertificate(CERTIFICATE_FILE_NAME);
    socket->setPrivateKey(KEY_FILE_NAME, QSsl::Rsa, QSsl::Pem, PASS_PHRASE);
    // Первыми инициализируем SSL-рукопожатие
    socket->startServerEncryption();
    stream = new QTextStream(socket);


    connect(socket, SIGNAL(encrypted()), SLOT([]()
    {
        qDebug() << "Запущено защищённое соединение!";
    }));
    connect(socket, SIGNAL(encryptedBytesWritten(qint64 written)),
            SLOT([](qint64 written)
            {
                qDebug() <<QString("По защищённому соединению было передано %1 байт")
                .arg(written);
            }));

    connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), SLOT(processIncomingMessage()));

    qDebug() << "Новое клиентское подключение создано!";
}



ClientConnection::~ClientConnection()
{
    socket->close();
    emit closed();
    delete socket;
    delete stream;
    qDebug() << "Клиентское подключение разорвано!";
}

void ClientConnection::close()
{
    socket->close();
}

void ClientConnection::sendResponse(QString response)
{
    *stream << response;
    stream->flush();
}

QString ClientConnection::receiveIncomingMessage()
{
    /* Корректно принимает сообщение от клиента целиком.
     * Может случиться так, что сообщение будет разбито на куски, которые будут приняты друг за другом
     * Именно поэтому мы склеивам сообщение из разных кусочков
     * Сообщения передаются в кодировке UTF-16
     */
    static QString clientMessage="";
    clientMessage += stream->readAll();

    if(!clientMessage.endsWith(END_OF_COMMAND))
    {
        // Пустая строка означает, что сообщение принято не до конца
        return "";
    }
    QString result{clientMessage};
    // Очищаем прочитанное сообщение, чтобы быть готовым прочесть новое
    clientMessage.clear();
    return result;
}

QStringList ClientConnection::parseMessage(QString clientMessage)
{
    // Избавляемся от ненужных пустых символов
    clientMessage = clientMessage.trimmed();
    QTextStream argStream(&clientMessage);
    QStringList result;
    // Разбиваем сообщение на фрагмента: команду и ее аргументы
    while(!argStream.atEnd())
    {
        QString arg;
        argStream >> arg;
        result.append(arg);
    }
    return result;
}

CommandType ClientConnection::getCommandType(const QString &commandName)
{
    if(commandName == LOGIN_COMMAND)
    {
        return CommandType::LogIn;
    }
    else if(commandName == REGISTRATION_COMMAND)
    {
        return CommandType::Registration;
    }
    return CommandType::Unspecified;
}

void ClientConnection::processCommand(QStringList messageParts)
{
    CommandType command = getCommandType(messageParts[0]);
    switch(command)
    {
        case CommandType::LogIn:
        {
            // Команда аутентификации принимает два параметра
            if(messageParts.length() == 3)
            {
                qDebug() << "Обрабатываем команду аутентификации";
                emit logInCommandSent(messageParts[1], messageParts[2]);
            }
            else
            {
                qDebug() << "В команде аутентификации указано неверное количество аргументов";
                sendResponse("- неверное количество аргументов\r\n");
            }
            break;
        }
        case CommandType::Registration:
        {
            /* Команда регистрации принимает 4 параметра
             * имя, фамилия, адрес электронной почты, пароль
             */
            if(messageParts.length() == 5)
            {
                qDebug() << "Обрабатываем команду регистрации";
                emit registrationCommandSent(messageParts[1], messageParts[2],
                        messageParts[3], messageParts[4]);
            }
            else
            {
                qDebug() << "В команде регистрации указано неверное количество аргументов";
                sendResponse("- неверное количество аргументов\r\n");
            }
            break;
        }
        case CommandType::Unspecified:
        {
            qDebug() << "Получена неизвестная команда "<< messageParts[0];
            break;
        }
    }
}

void ClientConnection::processIncomingMessage()
{
    QString clientMessage = receiveIncomingMessage();
    // В случае, если сообщение ещё не обработано
    if(clientMessage.isEmpty())
    {
        return;
    }
    qDebug() << "Пользователь отправил сообщение: "<<clientMessage;


    // Разбиваем входящую строку на фрагменты: команда и аргументы
    QStringList messageParts = parseMessage(clientMessage);
    qDebug() << messageParts;

    // Пустые команды не обрабатываем
    if(messageParts.length() > 1)
    {
        processCommand(messageParts);
    }
}

void ClientConnection::setStatusFlag(unsigned long long flag)
{
    statusFlags |= flag;
}


