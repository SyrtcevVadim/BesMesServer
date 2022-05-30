#ifndef USER_H
#define USER_H

#include<QString>
#include<QDate>

struct User
{
public:
    qint64 userId;
    /// Имя
    QString firstName;
    /// Фамилия
    QString lastName;
    /// Почта
    QString email;
    /// Очищает все поля структуры
    void clear();
};

#endif // USER_H
