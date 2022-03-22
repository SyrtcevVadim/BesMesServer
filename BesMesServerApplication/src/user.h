#ifndef USER_H
#define USER_H

#include<QString>
#include<QDate>

struct User
{
public:
    /// Имя
    QString firstName;
    /// Фамилия
    QString lastName;
    /// Почта
    QString email;
    /// Пароль от аккаунта в системе BesMes
    QString password;

    QString gender;
    QDate birthdate;
    /// Очищает все поля структуры
    void clear();
};

#endif // USER_H
