#ifndef EMAILSENDER_H
#define EMAILSENDER_H
// Автор: Сырцев Вадим Игоревич
#include <QThread>

class EmailSender : public QThread
{
    Q_OBJECT
public:
    EmailSender(QObject *parent = nullptr);

private:
};

#endif // EMAILSENDER_H
