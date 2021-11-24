#include "emailsender.h"


QRandomGenerator EmailSender::generator(QDateTime::currentSecsSinceEpoch());

EmailSender::EmailSender(const QString &recipientEmail,
                         BesConfigEditor *emailSenderConfigEditor,
                         QObject *parent)
    : QThread(parent),
      emailSenderConfigEditor(emailSenderConfigEditor),
      recipientEmail(recipientEmail)
{
    socket = new QSslSocket();
    stream = new QTextStream(socket);

    // Выбираем почту для отправки сообщений
    QMap<QString, QString> senders = emailSenderConfigEditor->getMap("senderEmails");
    qDebug() << senders;
    // Индекс почты отправителя в ассоциативном массиве
    int chosenSenderIndex = generator.bounded(0,senders.keys().length());
    int counter{0};
    //TODO ПЕРЕДЕЛАТЬ, ИТЕРАТОРЫ НЕ РАБОТАЮТ БЕЗ БРЯКИ
    for(const QString &sender: senders.keys())
    {
        if(counter == chosenSenderIndex)
        {
            senderEmail = sender;
            senderPassword = senders[senderEmail];
            qDebug() << "Устанавливаем почту "<<senderEmail <<" " << senderPassword;
            break;
        }
    }

    qDebug() << "Для отправки выбрана почта "<<senderEmail;

    connect(socket, SIGNAL(readyRead()), SLOT(processAnswer()));
}

EmailSender::~EmailSender()
{
    delete socket;
    delete stream;
}

void EmailSender::connectToSmtpServer()
{
    socket->connectToHostEncrypted(emailSenderConfigEditor->getString("smtpServerAddress"),
                                   emailSenderConfigEditor->getInt("smtpServerPort"));
    if(!socket->waitForConnected(2000))
    {
        qDebug() << "Ошибка! Не получается подключиться к "<<emailSenderConfigEditor->getString("smtpServerAddress")<<": "<<
                    emailSenderConfigEditor->getInt("smtpServerPort");
    }
    currentState=CommunicationStates::INITIALIZATION;
}

void EmailSender::sendVerificationCode(const QString &userName,
                                       const QString &verificationCode)
{
    currentEmailType = EmailType::EmailWithVerificationCode;
    this->userName=userName;
    this->verificationCode=verificationCode;

    // Подключаемся к smtp-серверу
    connectToSmtpServer();
    start();
}


void EmailSender::run()
{
    exec();
}

QString EmailSender::getMessage()
{
    QString result{};
    result += "To: "+recipientEmail+"\r\n";
    result += "From: "+senderEmail +"\r\n";
    switch(currentEmailType)
    {
        case EmailType::EmailWithVerificationCode:
        {
            result += "Subject: "+emailSenderConfigEditor->getString("verificationEmailTitle")+"\r\n";

            // Формируем тело сообщения
            QString body{};
            for(const QString &line:emailSenderConfigEditor->getStringList("verificationEmailBody"))
            {
                body+=line;
            }
            // В теле письма отправки кода верификации содержится имя пользователя и код верификации
            body = body.arg(userName, verificationCode);
            result += body+"\r\n.\r\n";
            break;
        }
    }
    return result;
}

void EmailSender::processAnswer()
{
    QString response="";
    QString currentInputLine;
    do
    {
        currentInputLine = socket->readLine();
        response+=currentInputLine;
    }while(socket->canReadLine()&&currentInputLine[3]!= ' ');

    qDebug() << "От сервера получен ответ: "<<response;

    currentInputLine.truncate(3);
    qDebug() << currentInputLine;
    SmtpAnswerCode responseCode = (SmtpAnswerCode)currentInputLine.toInt();

    if(responseCode == SmtpAnswerCode::READY &&
            currentState == CommunicationStates::INITIALIZATION)
    {
        *stream<< "EHLO BesMes\r\n";
        stream->flush();
        currentState = CommunicationStates::AUTHORIZATION;
    }
    else if(responseCode == SmtpAnswerCode::SUCCESS &&
            currentState == CommunicationStates::AUTHORIZATION)
    {
        *stream << "AUTH LOGIN\r\n";
        stream->flush();
        currentState = CommunicationStates::SENDING_USER_NAME;
    }
    else if(responseCode == SmtpAnswerCode::SEND_LOGIN &&
            currentState == CommunicationStates::SENDING_USER_NAME)
    {
        *stream << senderEmail.toUtf8().toBase64()<<"\r\n";
        stream->flush();
        currentState = CommunicationStates::SENDING_USER_PASSWORD;
    }
    else if(responseCode == SmtpAnswerCode::SEND_PASSWORD &&
            currentState == CommunicationStates::SENDING_USER_PASSWORD)\
    {
        *stream << senderPassword.toUtf8().toBase64()<<"\r\n";
        stream->flush();
        currentState = CommunicationStates::SENDING_SENDER_MAIL;
    }
    else if(responseCode == SmtpAnswerCode::START_SENDING_EMAIL&&
            currentState == CommunicationStates::SENDING_SENDER_MAIL)
    {
        *stream << "MAIL FROM:<"<<senderEmail<<">\r\n";
        stream->flush();
        currentState = CommunicationStates::SENDING_RECIPIENT_MAIL;
    }
    else if(responseCode == SmtpAnswerCode::SUCCESS &&
            currentState == CommunicationStates::SENDING_RECIPIENT_MAIL)
    {
        *stream << "RCPT TO:<"<<recipientEmail<<">\r\n";
        stream->flush();
        currentState = CommunicationStates::PREPARING_FOR_SENDING;
    }
    else if(responseCode == SmtpAnswerCode::SUCCESS &&
            currentState == CommunicationStates::PREPARING_FOR_SENDING)
    {
        *stream << "DATA\r\n";
        stream->flush();
        currentState = CommunicationStates::SENDING_EMAIL;
    }
    else if(responseCode == SmtpAnswerCode::SEND_MESSAGE &&
            currentState == CommunicationStates::SENDING_EMAIL)
    {
        *stream << getMessage();
        stream->flush();
        currentState = CommunicationStates::CLOSING_CONNECTION;
    }
    else if(responseCode == SmtpAnswerCode::SUCCESS &&
            currentState == CommunicationStates::CLOSING_CONNECTION)
    {
        *stream << "QUIT\r\n";
        stream->flush();
        exit();
        deleteLater();
    }
}


