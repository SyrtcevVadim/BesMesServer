#include <QDebug>
#include "serverworker.h"
#include "clientconnection.h"


// Изначально нет созданных объектов серверных рабочих
unsigned int ServerWorker::createdObjectCounter = 0;

ServerWorker::ServerWorker(ConfigFileEditor *configParameters,
                           QObject *parent):
    QThread(parent),
    configParameters(configParameters)
{

    id = createdObjectCounter++;
    initCounters();
    // Создаём соединение с базой данных
    dbConnection = new DatabaseConnection(QString("%1%2").arg((*configParameters)["user_name"], QString().setNum(id)));
}

void ServerWorker::configureDBConnection()
{
    dbConnection->setDatabaseAddress((*configParameters)["database_address"],
            (*configParameters)["database_port"].toInt());
    dbConnection->setUser((*configParameters)["user_name"], (*configParameters)["password"]);
    dbConnection->setDatabaseName();
}


unsigned long long ServerWorker::getHandlingConnectionsCounter()
{
    return handlingConnectionsCounter;
}

void ServerWorker::addClientConnection(qintptr socketDescriptor)
{
    ++handlingConnectionsCounter;
    ClientConnection *incomingConnection = new ClientConnection(socketDescriptor);
    // Связываем клиентское подключение с рабочим потоком
    // После разрыва пользовательского соединения уведомляем об этом рабочий поток
    connect(incomingConnection, SIGNAL(closed()), SIGNAL(clientConnectionClosed()));
    // После разрыва пользовательского соединения уменьшаем счётчик
    connect(incomingConnection, SIGNAL(closed()), SLOT(decreaseHandlingConnectionsCounter()));

    // Обрабатывает команду аутентификации
    connect(incomingConnection, SIGNAL(logInCommandSent(QString,QString)),
            SLOT(processLogInCommand(QString,QString)));
    // Обрабатывает команду регистрации
    connect(incomingConnection, SIGNAL(registrationCommandSent(QString,QString,QString,QString)),
            SLOT(processRegistrationCommand(QString,QString,QString,QString)));

    // При остановке рабочего потока должны быть разорваны все пользовательские соединения
    connect(this, SIGNAL(finished()),
            incomingConnection, SLOT(close()));

    incomingConnection->sendResponse("+ Привет! Вы подключены к серверу BesMesServer\r\n");
}

void ServerWorker::decreaseHandlingConnectionsCounter()
{
    --handlingConnectionsCounter;
    qDebug() << QString("Клиент в потоке %1 разорвал соединение. На данном потоке осталось %2 клиентов")
                .arg(QString().setNum(id), QString().setNum(handlingConnectionsCounter));
}

void ServerWorker::processLogInCommand(QString email, QString password)
{
    qDebug() << QString("Пользователь %1 хочет войти в систему!").arg(email);
    ClientConnection *client = (ClientConnection*)sender();
    // Проверяем, есть ли такой пользователь в БД
    if(dbConnection->userExists(email, password))
    {
        qDebug() << "Пользователь вошел в аккаунт";
        // Запоминаем в флаге статуса, что пользователь прошел процесс аутентификации
        client->setStatusFlag(LOGGED_IN_SUCCESSFULLY);
        client->sendResponse(QString("+ Вы успешно вошли в систему\r\n"));
    }
    else
    {
        qDebug() << "Пользователь не вошел в аккаунт";
        client->sendResponse(QString("- Не существует аккаунта с таким логином и паролем\r\n"));
    }
}

void ServerWorker::processRegistrationCommand(QString firstName, QString lastName,
                                              QString email, QString password)
{
    qDebug() << QString("Обрабатываем команду регистрации для пользвоателя %1 %2").arg(firstName, lastName);
    // Регистрируем пользователя, если нет пользователей с такой почтой
    if(!dbConnection->userExists(email))
    {
        ClientConnection *client = (ClientConnection*)sender();
        if(dbConnection->addNewUser(firstName, lastName, email, password))
        {
            client->sendResponse("+ Вы успешно зарегистрировали новый аккаунт\r\n");
        }
        else
        {
            client->sendResponse("- Не удалось зарегистрировать новый аккаунт\r\n");
        }
    }

}

void ServerWorker::run()
{
    qDebug() << QString("Поток %1 запущен").arg(id);
    configureDBConnection();
    dbConnection->open();
    exec();
}

void ServerWorker::initCounters()
{
    handlingConnectionsCounter=0;
}

