#ifndef CHAT_H
#define CHAT_H
#include<QString>
#include<QVector>
#include "message.h"

struct Chat
{
    qint64 chatId;
    QString chatTitle;
    QVector<Message> unreadMessages;
};

#endif // CHAT_H
