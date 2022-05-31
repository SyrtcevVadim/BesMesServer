#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>
struct Message
{
    QString body;
    qint64 senderId;
    qint64 sendingTimestamp;
};

#endif // MESSAGE_H
