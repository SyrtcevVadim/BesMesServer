#include <QDebug>
#include "besProtocol.h"
#include "serverworker.h"
#include "clientconnection.h"
#include "emailsender.h"


// Изначально нет созданных объектов серверных рабочих
unsigned int ServerWorker::createdObjectCounter = 0;
QRandomGenerator ServerWorker::generator(QDateTime::currentSecsSinceEpoch());

ServerWorker::ServerWorker(BesLogSystem *logSystem,
                           QObject *parent):
    QThread(parent),
    logSystem(logSystem)
{
    id = createdObjectCounter++;
    configureConfigEditors();
    configureLogSystem();
    initCounters();
}

ServerWorker::~ServerWorker()
{
    delete databaseConnection;
    delete serverConfigEditor;
}

void ServerWorker::quit()
{
    // При остановке рабочего потока нужно удалить соединение с базой данных
    delete databaseConnection;
    QThread::quit();
}

void ServerWorker::configureConfigEditors()
{
    serverConfigEditor = new BesConfigEditor(SERVER_CONFIG_FILE_NAME);
}

void ServerWorker::configureDatabaseConnection()
{
    databaseConnection = new DatabaseConnection(QString("connection%1")
                                           .arg(id));
}

void ServerWorker::configureLogSystem()
{
    // Сохраняем в журнале сообщений следующие события
    connect(this, SIGNAL(databaseConnectionEstablished(int)),
            logSystem, SLOT(logDatabaseConnectionEstablishedMessage(int)));
    connect(this, SIGNAL(databaseConnectionFailed(int)),
            logSystem, SLOT(logDatabaseConnectionFailedMessage(int)));

    connect(this, SIGNAL(clientLoggedIn(QString)),
            logSystem, SLOT(logClientLoggedInMessage(QString)));

    connect(this, SIGNAL(clientFailedAuthentication(QString)),
            logSystem, SLOT(logClientFailedAuthentication(QString)));
    connect(this, SIGNAL(verificationCodeWasSent(QString)),
            logSystem, SLOT(logVerificationCodeWasSent(QString)));
    connect(this, SIGNAL(clientWasRegistered(QString)),
            logSystem, SLOT(logClientWasRegistered(QString)));
    connect(this, SIGNAL(clientSentWrongVerificationCode(QString,QString)),
            logSystem, SLOT(logClientSentWrongVerificationCode(QString,QString)));
    connect(this, SIGNAL(clientUsedOccupiedEmailForRegistration(QString)),
            logSystem, SLOT(logClientUsedOccupiedEmailForRegistration(QString)));
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
    // После разрыва соединения с клиентским приложением, нужно удалить объект
    connect(incomingConnection, SIGNAL(closed()), incomingConnection, SLOT(deleteLater()));

    // Обрабатывает команду аутентификации
    connect(incomingConnection, SIGNAL(logInCommandSent(QString,QString)),
            SLOT(processLogInCommand(QString,QString)));
    // Обрабатывает команду регистрации
    connect(incomingConnection, SIGNAL(registrationCommandSent(QString,QString,QString,QString)),
            SLOT(processRegistrationCommand(QString,QString,QString,QString)));
    // Обрабатывает команду верификации регистрации
    connect(incomingConnection, SIGNAL(verificationCommandSent(QString)),
            SLOT(processVerificationCommand(QString)));
    // Обрабатываем команду авторизации администратора
    connect(incomingConnection, SIGNAL(superLogInCommandSent(QString,QString)),
            SLOT(processSuperLogInCommand(QString,QString)));

    // При остановке рабочего потока должны быть разорваны все пользовательские соединения
    connect(this, SIGNAL(finished()),
            incomingConnection, SLOT(close()));

    incomingConnection->sendResponse(QString("+ %1").arg(GREETING_MESSAGE));
}

void ServerWorker::decreaseHandlingConnectionsCounter()
{
    --handlingConnectionsCounter;
    qDebug() << QString("Клиент в потоке %1 разорвал соединение. На данном потоке осталось %2 клиентов")
                .arg(QString().setNum(id), QString().setNum(handlingConnectionsCounter));
}

void ServerWorker::processLogInCommand(QString email, QString password)
{
    qDebug() << QString("Пользователь %1 пытается пройти аутентификацию с паролем %2")
                .arg(email, password);

    ClientConnection *client = (ClientConnection*)sender();
    // Проверяем, есть ли такой пользователь в БД
    if(databaseConnection->userExists(email, password))
    {
        qDebug() << QString("Пользователь %1 прошёл аутентификацию").arg(email);
        emit clientLoggedIn(email);
        // Запоминаем в флаге статуса, что пользователь прошел процесс аутентификации
        client->setStatusFlag(LOGGED_IN_SUCCESSFULLY);
        client->sendResponse(QString("+ Вы успешно вошли в систему"));
    }
    else
    {
        qDebug() << QString("Пользователь %1 не прошёл аутентификацию").arg(email);
        emit clientFailedAuthentication (email);
        client->sendResponse(QString("- %1 Не существует аккаунта с таким логином и паролем")
                             .arg(WRONG_USERNAME_OR_PASSWORD_ERROR));
    }
}

QString ServerWorker::generateVerificationCode()
{
    // Допустим, сейчас код будет состоять из 6 знаков
    qint32 result = generator.bounded((int)pow(10, VERIFICATION_CODE_LENGTH-1),
                                      (int)pow(10,VERIFICATION_CODE_LENGTH)-1);
    return QString().setNum(result);
}

void ServerWorker::processRegistrationCommand(QString firstName, QString lastName,
                                              QString email, QString password)
{
    qDebug() << QString("Пользователь \"%1 %2\" с почтой \"%3\" и паролем \"%4\" пытается зарегистрироваться")
                .arg(firstName, lastName, email, password);
    ClientConnection *client = (ClientConnection*)sender();
    // Регистрируем пользователя, если нет пользователей с такой почтой
    if(!databaseConnection->userExists(email))
    {
        // Генерируем код верификации для пользователя
        QString verificationCode = generateVerificationCode();

        client->sendResponse(QString("+ Вам на почту отправлен код подтверждения регистрации"));
        qDebug()<<"Пользователю на почту отправлен код верификации "<<verificationCode;
        EmailSender *emailSender = new EmailSender(email);
        emailSender->sendVerificationCode(firstName, verificationCode);

        emit verificationCodeWasSent(email);

        // Сохраняем пользовательские данные
        User &clientData = client->user;
        clientData.firstName = firstName;
        clientData.lastName = lastName;
        clientData.email = email;
        clientData.password = password;
        client->verificationCode=verificationCode;
    }
    else
    {
        qDebug() << "При регистрации пользователь указал используемую почту "<< email;
        emit clientUsedOccupiedEmailForRegistration(email);

        client->sendResponse(QString("- %1 На данную почту уже зарегистрирован аккаунт")
                             .arg(EMAIL_OCCUPIED_ERROR));
    }

}

void ServerWorker::processVerificationCommand(QString code)
{
    qDebug() << "Обрабатываем команду верификации регистрации с кодом "<<code;
    ClientConnection *client = (ClientConnection*)sender();
    // Проверяем правильность кода
    if(client->checkVerificationCode(code))
    {

        client->sendResponse(QString("+ Код верификации был принят."));
        User clientData = client->user;
        qDebug() << "Пользователь "<<clientData.email <<" прислал верный код! Верификация прошла успешно";
        if(databaseConnection->addNewUser(clientData.firstName,
                                    clientData.lastName,
                                    clientData.email,
                                    clientData.password))
        {
            emit clientWasRegistered(clientData.email);
            qDebug() << "зарегистрирован новый аккаунт пользотеля: "<<clientData.email;
            client->sendResponse(QString("+ Вы успешно зарегистрировали новый аккаунт"));
        }
        else
        {
            qDebug() << "По каким-то внутренним причинам не удалось зарегистрировать новый аккаунт";
            client->sendResponse(QString("- %1 Не удалось зарегистрировать новый аккаунт из-за ошибки, возникшей на сервере")
                                 .arg(REGISTRATION_INNER_ERROR));
        }

    }
    else
    {
        qDebug() <<"Пользователь "<<client->user.email <<" прислал неверный код верификации: "<<code;
        emit clientSentWrongVerificationCode(client->user.email, code);
        client->sendResponse(QString("- %1 Отправлен неверный код верификации")
                             .arg(WRONG_VERIFICATION_CODE_ERROR));
    }
    // Очищаем предыдущие данны о пользователе
    client->user.clear();
}

void ServerWorker::processSuperLogInCommand(QString login, QString password)
{
    ClientConnection *client = (ClientConnection*)sender();
    // Сравниваем данные суперпользователя из файла конфигурации с данными, предоставленными пользователем
    if(serverConfigEditor->getString("superUserLogin") == login &&
            serverConfigEditor->getString("superUserLogin") == password)
    {
        // Авторизация прошла успешно
        qDebug() << "Администратор авторизовался!";
        client->sendResponse(QString("+ Вы авторизовались в системе как администратор!"));
    }
    else
    {
//        emit logMessage(QString("Попытка авторизации. Предоставлены данные:"
//                                "логин: \"%1\" пароль: \"%2\"")
//                        .arg(login, password));
        // В целях "безопасности" отсылаем такой ответо
        client->sendResponse("- Не существует такой команды");
        // Устанавливаем флаг того, что данный пользователь является администратором
        client->setStatusFlag(IS_ADMINISTRATOR);
    }
}


void ServerWorker::run()
{
    // Обновляем параметры сервера из конфигурационного файла
    serverConfigEditor->retrieveParameters();
    qDebug() << QString("Поток %1 запущен").arg(id);
    configureDatabaseConnection();
    databaseConnection->open();
    if(databaseConnection->isActive())
    {
        emit databaseConnectionEstablished(id);
    }
    else
    {
        emit databaseConnectionFailed(id);
    }
    exec();
}

void ServerWorker::initCounters()
{
    handlingConnectionsCounter=0;
}

