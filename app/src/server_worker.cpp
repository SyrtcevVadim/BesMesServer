#include <QDebug>

#include "server_worker.h"
#include "client_connection.h"

// Изначально нет созданных объектов серверных рабочих
unsigned int ServerWorker::createdObjectCounter = 0;

ServerWorker::ServerWorker(QObject *parent):
    QThread(parent)
{
    id = createdObjectCounter++;
    initCounters();
}

ServerWorker::~ServerWorker()
{
    qDebug() << "Удаляем рабочего";
    delete databaseConnection;
}

void ServerWorker::quit()
{
    // При остановке рабочего потока нужно удалить соединение с базой данных
    delete databaseConnection;
    QThread::quit();
}

void ServerWorker::configureDatabaseConnection()
{
    databaseConnection = new DatabaseConnection(QString("connection%1")
                                           .arg(id));
}

unsigned long long ServerWorker::getHandlingConnectionsCounter()
{
    return handlingConnectionsCounter;
}

void ServerWorker::addClientConnection(qintptr socketDescriptor)
{
    ++handlingConnectionsCounter;
    ClientConnection *incomingConnection = new ClientConnection(socketDescriptor, this);
    // Связываем клиентское подключение с рабочим потоком
    // После разрыва пользовательского соединения уведомляем об этом рабочий поток
    connect(incomingConnection, SIGNAL(closed()), SIGNAL(clientConnectionClosed()));
    // После разрыва пользовательского соединения уменьшаем счётчик
    connect(incomingConnection, SIGNAL(closed()), SLOT(decreaseHandlingConnectionsCounter()));
    // После разрыва соединения с клиентским приложением, нужно удалить объект
    connect(incomingConnection, SIGNAL(closed()), incomingConnection, SLOT(deleteLater()));

    // При остановке рабочего потока должны быть разорваны все пользовательские соединения
    connect(this, SIGNAL(finished()),
            incomingConnection, SLOT(close()));
}

void ServerWorker::decreaseHandlingConnectionsCounter()
{
    --handlingConnectionsCounter;
    qDebug() << QString("Клиент в потоке %1 разорвал соединение. На данном потоке осталось %2 клиентов")
                .arg(QString().setNum(id), QString().setNum(handlingConnectionsCounter));
}

void ServerWorker::run()
{
    qDebug() << QString("Поток %1 запущен").arg(id);
    configureDatabaseConnection();
    databaseConnection->open();
    exec();
}

void ServerWorker::initCounters()
{
    handlingConnectionsCounter=0;
}

bool ServerWorker::verifyLogIn(const QString &email, const QString &password)
{
    return databaseConnection->verifyLogIn(email, password);
}

qint64 ServerWorker::getUserId(const QString &email)
{
    return databaseConnection->getUserId(email);
}

bool ServerWorker::registerNewUser(const QString &firstName,
                                     const QString &lastName,
                                     const QString &email,
                                     const QString &password)
{
    return databaseConnection->registerNewUser(firstName, lastName, email, password);
}

QVector<User> ServerWorker::getListOfUsers()
{
    return databaseConnection->getListOfUsers();
}

QVector<Chat> ServerWorker::getListOfChats(qint64 userId)
{
    return databaseConnection->getListOfChats(userId);
}

qint64 ServerWorker::createNewChat(qint64 ownerId, const QString &chatTitle)
{
    return databaseConnection->createNewChat(ownerId, chatTitle);
}
bool ServerWorker::deleteChat(qint64 chatId)
{
    return databaseConnection->deleteChat(chatId);
}

bool ServerWorker::inviteToChat(qint64 chatId, qint64 userId)
{
    return databaseConnection->inviteTochat(chatId, userId);
}

bool ServerWorker::kickFromChat(qint64 chatId, qint64 userId)
{
    return databaseConnection->kickFromChat(chatId, userId);
}

QVector<qint64> ServerWorker::getUsersInChat(qint64 chatId)
{
    return databaseConnection->getUsersInChat(chatId);
}

qint64 ServerWorker::sendMessage(qint64 chatId, const QString &messageBody, qint64 senderId)
{
    return databaseConnection->sendMessage(chatId, messageBody, senderId);
}

QVector<Chat> ServerWorker::synchronize(qint64 userId, qint64 lastMessageTimestamp)
{
    QVector<Chat> chatsWithUnreadMessages = databaseConnection->getChatsWithUnreadMessages(userId, lastMessageTimestamp);

    for (Chat &currentChat: chatsWithUnreadMessages)
    {
        currentChat.unreadMessages = databaseConnection->getUnreadMessages(currentChat.chatId, lastMessageTimestamp);
    }
    return chatsWithUnreadMessages;
}
