#ifndef BES_PROTOCOL_H
#define BES_PROTOCOL_H
/*
 * Автор: Сырцев Вадим Игоревич,
 * Воропаев Владимир Геннадьевич,
 * Жужликова Екатерина Алексеевна
 * Группа 03ПГ
*/

// Сообщение, которое отправляет каждому новому подключённому соединению
#define GREETING_MESSAGE "Привет! Вы успешно подключились к серверу BesMesServer! Пройдите аутентификацию или зарегистрируйтесь!"

// Общие ошибки
#define NOT_ENOUGH_ARGS_ERROR 1

/* ------------------------------------------------------------
// СПИСОК КОМАНД ОБЩЕНИЯ КЛИЕНТСКОГО ПРИЛОЖЕНИЯ С СЕРВЕРНЫМ И
// ВОЗМОЖНЫХ ОТВЕТОВ СЕРВЕРА НА ОШИБКИ
// ------------------------------------------------------------
*/

#define QUERY_NAME_KEY "тип_запроса"
#define QUERY_RESPONSE_KEY "код_ответа"
#define USER_ID_KEY "ид_пользователя"
#define USER_FIRST_NAME_KEY "имя"
#define USER_LAST_NAME_KEY "фамилия"
#define USER_EMAIL_KEY "почта"
#define USER_PASSWORD_KEY "пароль"

#define USERS_KEY "пользователи"
#define CHATS_KEY "чаты"

#define CHAT_ID_KEY "ид_чата"
#define CHAT_TITLE_KEY "название"

#define MESSAGE_BODY_KEY "тело_сообщения"
#define SENDER_ID_KEY "ид_отправителя"

// Команда аутентификации существующего пользователя
#define LOGIN_QUERY "ЛОГИН"
// Возможные ошибки:
// Не существует пользователя
#define WRONG_USERNAME_OR_PASSWORD_ERROR 2
// Команда регистрации нового пользователя
#define REGISTRATION_QUERY "РЕГИСТРАЦИЯ"
// Возможные ошибки:
// Пользователь с таким адресом электронной почты уже существует
#define EMAIL_OCCUPIED_ERROR 3

#define GET_USERS_LIST_QUERY "СПИСОКПОЛЬЗОВАТЕЛЕЙ"

#define GET_CHATS_LIST_QUERY "ЧАТЛИСТ"

#define GET_UNREAD_MESSAGES_QUERY "ЧАТСООБЩ"

#define SEND_MESSAGE_QUERY "ОТПРСООБЩ"

#define CREATE_CHAT_QUERY "ЧАТСОЗДАТЬ"

#define DELETE_CHAT_QUERY "ЧАТУДАЛ"

#define INVITE_TO_CHAT_QUERY "ЧАТПРИГЛ"

#define KICK_FROM_CHAT_QUERY "ЧАТИСКЛ"

#define REFRESH_CHAT_QUERY "ЧАТОБНОВ"

#define SYNCHRONIZATION_QUERY "СИНХР"
#endif // BES_PROTOCOL_H
