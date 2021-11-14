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
    /// Пол
    QString gender;
    /// Адрес электронной почты
    QString email;
    /// Пароль от аккаунта пользователя
    QString password;
    /// Дата рождения
    QDate birthdate;

};

#endif // USER_H
