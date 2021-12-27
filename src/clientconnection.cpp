#include<QFile>
#include<QSslKey>
#include<QSslCertificate>
#include "clientconnection.h"
#include "besconfigreader.h"
#include "beslogsystem.h"

QSslConfiguration ClientConnection::sslConfiguration;

void ClientConnection::initSslConfiguration()
{
    sslConfiguration.setProtocol(QSsl::TlsV1_3OrLater);
    BesConfigReader *configs = BesConfigReader::getInstance();

    QString pathToPrivateKey{configs->getString("security","path_to_private_key")};
    QFile *privateKeyFile = new QFile(pathToPrivateKey);
    // Нужно открыть файл, чтобы QSslKey смог прочесть его содержимое
    if(!privateKeyFile->open(QIODevice::ReadOnly))
    {
        BesLogSystem::getInstance()->logUnableToOpenPrivateKeyMessage(pathToPrivateKey);
    }
    QSslKey privateKey(privateKeyFile, QSsl::Rsa,
                       QSsl::Pem, QSsl::PrivateKey,
                       configs->getString("security", "pass_phrase").toUtf8());
    privateKeyFile->close();
    sslConfiguration.setPrivateKey(privateKey);
    // Устанавливает сертификат
    QString pathToCertificate{configs->getString("security", "path_to_certificate")};
    QFile *certificateFile = new QFile(pathToCertificate);
    if(!certificateFile->open(QIODevice::ReadOnly))
    {
        BesLogSystem::getInstance()->logUnableToOpenCertificateMessage(pathToCertificate);
    }
    QSslCertificate certificate(certificateFile, QSsl::Pem);
    sslConfiguration.setLocalCertificate(certificate);
    certificateFile->close();
}

ClientConnection::ClientConnection(qintptr socketDescriptor, QObject *parent) : QObject(parent)
{
    // Обнуляем все флаги статуса
    statusFlags=0;
    // Создаём сокет защищённого уровня
    socket = new QSslSocket();
    socket->setSocketDescriptor(socketDescriptor);
    // Устанавливаем конфигурацию Ssl для серверного сокета
    socket->setSslConfiguration(sslConfiguration);
    // Первыми инициализируем SSL-рукопожатие
    socket->startServerEncryption();
    stream = new QTextStream(socket);

    connect(socket, SIGNAL(encrypted()), SLOT(sendGreetingMessage()));
    connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), SLOT(processIncomingMessage()));
}

void ClientConnection::sendGreetingMessage()
{
    sendResponse(QString("+ %1").arg(GREETING_MESSAGE));
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
    *stream << response << END_OF_MESSAGE;
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

    if(!clientMessage.endsWith(END_OF_MESSAGE))
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

Command ClientConnection::getCommandType(const QString &commandName)
{
    if(commandName == LOGIN_COMMAND)
    {
        return Command::LogIn;
    }
    else if(commandName == REGISTRATION_COMMAND)
    {
        return Command::Registration;
    }
    else if(commandName == VERIFICATION_COMMAND)
    {
        return Command::Verification;
    }
    else if(commandName == SUPER_LOGIN_COMMAND)
    {
        return Command::SuperLogIn;
    }
    return Command::Unspecified;
}

void ClientConnection::processCommand(QStringList messageParts)
{
    Command command = getCommandType(messageParts[0]);
    Error occuredError=Error::None;

    switch(command)
    {
        case Command::LogIn:
        {
            // Команда аутентификации принимает два параметра
            if(messageParts.length()-1 == LOGIN_REQUIRED_ARGS)
            {
                qDebug() << "Обрабатываем команду аутентификации";
                emit logInCommandSent(messageParts[1], messageParts[2]);
            }
            else
            {
                qDebug() << "В команде аутентификации указано неверное количество аргументов";
                occuredError = Error::Not_enought_args;
            }
            break;
        }
        case Command::Registration:
        {
            /* Команда регистрации принимает 4 параметра
             * имя, фамилия, адрес электронной почты, пароль
             */
            if(messageParts.length()-1 == REGISTRATION_REQUIRED_ARGS)
            {
                qDebug() << "Обрабатываем команду регистрации";
                emit registrationCommandSent(messageParts[1], messageParts[2],
                        messageParts[3], messageParts[4]);
            }
            else
            {
                qDebug() << "В команде регистрации указано неверное количество аргументов";
                occuredError = Error::Not_enought_args;
            }
            break;
        }
        case Command::Verification:
        {
            if(messageParts.length()-1 == VERIFICATION_REQUIRED_ARGS)
            {
                qDebug() << "Обрабатываем команду с кодом верификации регистрации";
                emit verificationCommandSent(messageParts[1]);
            }
            else
            {
                qDebug() << "В команде регистрации указано неверное количество аргументов";
                occuredError=Error::Not_enought_args;
            }
            break;
        }
        case Command::SuperLogIn:
        {
            if(messageParts.length()-1 == SUPER_LOGIN_REQUIRED_ARGS)
            {
                qDebug() << "Получена команда авторизации администратора";
                emit superLogInCommandSent(messageParts[1], messageParts[2]);
            }
            else
            {
                occuredError=Error::Not_enought_args;
            }
            break;
        }
        case Command::Unspecified:
        {
            qDebug() << "Получена неизвестная команда "<< messageParts[0];
            break;
        }
    }
    // Блок обработки ошибок
    switch(occuredError)
    {
        case Error::Not_enought_args:
        {
            sendResponse(QString("- %1 неверное количество аргументов")
                         .arg(NOT_ENOUGH_ARGS_ERROR));
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

void ClientConnection::setVerificationCode(const QString &code)
{
    verificationCode = code;
}

bool ClientConnection::checkVerificationCode(const QString &code)
{
    return code == verificationCode;
}
