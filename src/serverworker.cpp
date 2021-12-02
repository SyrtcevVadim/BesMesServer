#include <QDebug>
#include "besProtocol.h"
#include "serverworker.h"
#include "clientconnection.h"
#include "emailsender.h"


// Изначально нет созданных объектов серверных рабочих
unsigned int ServerWorker::createdObjectCounter = 0;
QRandomGenerator ServerWorker::generator(QDateTime::currentSecsSinceEpoch());

ServerWorker::ServerWorker(BesConfigEditor *serverConfigEditor,
                           BesConfigEditor *databaseConnectionConfigEditor,
                           BesConfigEditor *emailSenderConfigEditor,
                           BesLogSystem *logSystem,
                           QObject *parent):
    QThread(parent),
    serverConfigEditor(serverConfigEditor),
    databaseConnectionConfigEditor(databaseConnectionConfigEditor),
    emailSenderConfigEditor(emailSenderConfigEditor),
    logSystem(logSystem)
{
    id = createdObjectCounter++;
    configureLogSystem();
    initCounters();
}

ServerWorker::~ServerWorker()
{
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
    //emit logMessage(QString("Пользователь %1 хочет войти в систему").arg(email));
    ClientConnection *client = (ClientConnection*)sender();
    // Проверяем, есть ли такой пользователь в БД
    if(databaseConnection->userExists(email, password))
    {
        //emit logMessage(QString("Пользователь %1 успешно прошёл аутентификацию").arg(email));
        emit clientLoggedIn(email);
        // Запоминаем в флаге статуса, что пользователь прошел процесс аутентификации
        client->setStatusFlag(LOGGED_IN_SUCCESSFULLY);
        client->sendResponse(QString("+ Вы успешно вошли в систему"));
    }
    else
    {
        //emit logMessage(QString("Пользователь %1 не прошёл аутентификацию").arg(email));
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
    //emit logMessage(QString("Обрабатываем команду регистрации для пользвоателя %1 %2").arg(firstName, lastName));
    ClientConnection *client = (ClientConnection*)sender();
    // Регистрируем пользователя, если нет пользователей с такой почтой
    if(!databaseConnection->userExists(email))
    {
        // Генерируем код верификации для пользователя
        QString verificationCode = generateVerificationCode();
        //emit logMessage(QString("Пользователь должен отправить код подтверждения регистрации: %1")
        //              .arg(verificationCode));

        client->sendResponse(QString("+ Вам на почту отправлен код подтверждения регистрации"));
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
        emit clientUsedOccupiedEmailForRegistration(email);
        //emit logMessage(QString("Пользователь %1 %2 пытается зарегистрировать аккаунт на занятую почту %3")
        //                .arg(firstName, lastName, email));
        client->sendResponse(QString("- %1 На данную почту уже зарегистрирован аккаунт")
                             .arg(EMAIL_OCCUPIED_ERROR));
    }

}

void ServerWorker::processVerificationCommand(QString code)
{
//    emit logMessage(QString("Обрабатываем команду верификации с кодом %1")
//                    .arg(code));
    ClientConnection *client = (ClientConnection*)sender();
    // Проверяем правильность кода
    if(client->checkVerificationCode(code))
    {
        client->sendResponse(QString("+ Код верификации был принят! Пользователь зарегистрирован"));
        User clientData = client->user;
        if(databaseConnection->addNewUser(clientData.firstName,
                                    clientData.lastName,
                                    clientData.email,
                                    clientData.password))
        {
//            emit logMessage(QString("Зарегистрирован новый аккаунт на почту %1").
//                            arg(clientData.email));
            emit clientWasRegistered(clientData.email);
            client->sendResponse(QString("+ Вы успешно зарегистрировали новый аккаунт"));
        }
        else
        {
//            emit logMessage(QString("Пользователь %1 %2 пытался зарегистрировать аккаунт на почту %3."
//                                    " произошла внутренняя ошибка!")
//                            .arg(clientData.firstName,
//                                 clientData.lastName,
//                                 clientData.email));
            client->sendResponse(QString("- ?! Не удалось зарегистрировать новый аккаунт"));
        }

    }
    else
    {
        emit clientSentWrongVerificationCode(client->user.email, code);
        // TODO отправить сообщение пользователю
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
    qDebug() << QString("Поток %1 запущен").arg(id);
    configureDatabaseConnection();
    databaseConnection->open();
    if(databaseConnection->isActive())
    {
        emit databaseConnectionEstablished(id);
//        emit logMessage(QString("Рабочий поток %1 установил соединение с БД").arg(id));
    }
    else
    {
        emit databaseConnectionFailed(id);
//        emit logMessage(QString("Рабочий поток %1 не смог установить соединение с БД").arg(id));
    }
    exec();
}

void ServerWorker::initCounters()
{
    handlingConnectionsCounter=0;
}

