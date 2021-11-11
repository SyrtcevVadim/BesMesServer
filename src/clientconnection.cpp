#include "clientconnection.h"

#include<QRegExp>

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

void ClientConnection::receiveServerResponse(QString response)
{
    *stream << response;
    stream->flush();
}

void ClientConnection::processIncomingMessage()
{
    /* Корректно принимает пользовательское сообщение целиком.
     * Может случиться так, что сообщение пользователя не дойдет до отправителя
     * за один раз. Именно поэтому мы склеивам сообщение из разных кусочков
     */
    static QString clientMessage="";
    clientMessage += socket->readAll();
    if(!clientMessage.endsWith(END_OF_COMMAND))
    {
        return;
    }
    // Сообщение полностью получено
    qDebug() << "Пользователь отправил сообщение: "<<clientMessage;

    // Разбиваем входящую строку на фрагменты: команда и аргументы
    QStringList args;
    // Избавляемся от "\r\n"
    clientMessage = clientMessage.trimmed();
    QTextStream argStream(&clientMessage);
    while(!argStream.atEnd())
    {
        QString arg;
        argStream >> arg;
        args.append(arg);
    }
    qDebug() << args;
    // Команда HELLO всегда содержит 3 части: команда, имя, пароль
    if(args.length() == 3 && args[0]==HELLO_COMMAND)
    {
        qDebug() << "Отпралена команда для аутентификации";
        // Обработкой этого сигнала займётся серверный рабочий поток
        emit helloSaid(args[1], args[2]);
    }
    // Очищаем пользовательское соединение
    clientMessage = "";
}



