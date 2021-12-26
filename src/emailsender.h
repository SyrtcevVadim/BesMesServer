#ifndef EMAILSENDER_H
#define EMAILSENDER_H
// Автор: Сырцев Вадим Игоревич
#include <QThread>
#include <QSslSocket>
#include <QTextStream>
#include <QRandomGenerator>
#include"besconfigreader.h"

enum class  SmtpAnswerCode{READY=220,
                           SUCCESS=250,
                           SEND_LOGIN=334,
                           SEND_PASSWORD=334,
                           SEND_MESSAGE=354,
                           START_SENDING_EMAIL=235};

enum class  CommunicationStates{INITIALIZATION,
                                AUTHORIZATION,
                                SENDING_USER_NAME,
                                SENDING_USER_PASSWORD,
                                SENDING_SENDER_MAIL,
                                SENDING_RECIPIENT_MAIL,
                                PREPARING_FOR_SENDING,
                                SENDING_EMAIL,
                                CLOSING_CONNECTION};
class EmailSender : public QThread
{
    Q_OBJECT
public:
    EmailSender(const QString &recipientEmail,
                QObject *parent = nullptr);
    ~EmailSender();
    /// Сообщает объекту, что нужно отправить на почту получателя сообщение с кодом
    /// верификации
    void sendVerificationCode(const QString &userName,
                              const QString &verificationCode);

protected:
    void run();
private slots:
    /// Обрабатывает ответы от smtp-сервера
    void processAnswer();
private:
    /// Устанавливает подключение с SMTP-сервером
    void connectToSmtpServer();
    /// Формирует из файла конфигурации текст письма
    QString getMessage();

    enum class EmailType {EmailWithVerificationCode};

    /// Имя пользователя
    QString userName;
    /// Код верификации регистрации, который нужно отправить в письме
    QString verificationCode;
    /// Тип письма, которое будет отправлено в текуще сессии
    EmailType currentEmailType;
    /// Почта, с которой будет отправлено сообщение
    QString senderEmail;
    /// Пароль от почты отправителя
    QString senderPassword;
    /// Адрес электронной почты получателя
    QString recipientEmail;
    /// Сокет, по которому отправитель соединяется с smtp-сервером
    QSslSocket *socket;
    /// Текстовый поток, связанный с сокетом
    QTextStream *stream;
    /// Текущая ступень общения с smtp-сервером
    CommunicationStates currentState;
    /// Генератор псевдослучайных чисел. Используется для выбора почты,
    /// с которой будет отправлено сообщение пользователю
    static QRandomGenerator generator;
};

#endif // EMAILSENDER_H
