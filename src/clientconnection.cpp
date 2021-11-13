#include "clientconnection.h"

ClientConnection::ClientConnection(qintptr socketDescriptor, QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);

    stream = new QTextStream(socket);
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
    QStringList result;
    // Избавляемся от ненужных пустых символов
    clientMessage = clientMessage.trimmed();
    QTextStream argStream(&clientMessage);
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
    else
    {
        return CommandType::Unspecified;
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

    CommandType command = getCommandType(messageParts[0]);

    switch(command)
    {
        case CommandType::LogIn:
        {
            // Команда аутентификации принимает два параметра
            if(messageParts.length() == 3)
            {
                qDebug() << "Отправлена команда аутентификации";
                emit logInCommandSent(messageParts[1], messageParts[2]);
            }
            else
            {
                qDebug() << "В команде аутентификации указано неверное количество аргументов";
                sendResponse("- неверное количество аргументов\r\n");
            }
            break;
        }
        case CommandType::Unspecified:
        {
            qDebug() << "Получена неизвестная команда "<< messageParts[0];
        }
    }
}



