#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>
#include <QCryptographicHash>
#include "database_connection.h"

DatabaseConnection::DatabaseConnection(const QString &connectionName)
{
    this->connectionName=connectionName;
    configureDatabaseConnection();
}

DatabaseConnection::~DatabaseConnection()
{
    // Перед удалением объекта подключения к базе данных, нужно удалить физическое подключение
    besMesDatabase.close();
    besMesDatabase=QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}

void DatabaseConnection::configureDatabaseConnection()
{
    // Получаем доступ к параметрам конфигурации
    ConfigReader &config_reader = ConfigReader::getInstance();
    besMesDatabase = QSqlDatabase::addDatabase(USING_PLUGIN_NAME, connectionName);
    // Указываем ip-адрес устройства, на котором развёрнута база данных и порт, прослушиваемый базой данных
    besMesDatabase.setHostName(config_reader.getDatabaseHostAddress());
    besMesDatabase.setPort(config_reader.getDatabaseListeningPort());
    // Указываем имя базы данных, к которой хотим подключиться
    besMesDatabase.setDatabaseName(config_reader.getDatabaseName());
    // Указываем имя аккаунта, который мы будем использовать для подключения к базе данных
    besMesDatabase.setUserName(config_reader.getDatabaseUserName());
    besMesDatabase.setPassword(config_reader.getDatabaseUserPassword());
}

void DatabaseConnection::open()
{
    if(!besMesDatabase.open())
    {
        qDebug() << "Не смогли открыть подключение к базе данных";
    }
}

void DatabaseConnection::close()
{
    besMesDatabase.close();
}

bool DatabaseConnection::isEmailAvailable(const QString &email)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT is_email_available(:email)");
    query.bindValue(":email", email);
    query.exec();
    query.first();
    // Возвращает true, если данная почта свободна, иначе - false
    return query.value(0).toBool();
}

bool DatabaseConnection::verifyLogIn(const QString &email, const QString &password)
{
    // Хэшируем пароль
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Algorithm::Keccak_512);

    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT verify_log_in(:email, :hashed_password)");
    query.bindValue(":email", email);
    query.bindValue(":hashed_password", QLatin1String(hashedPassword.toHex()));
    query.exec();
    query.first();
    return query.value(0).toBool();
}

qint64 DatabaseConnection::getUserId(const QString &email)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT get_user_id(:email)");
    query.bindValue(":email", email);
    query.exec();
    query.first();
    return query.value(0).toInt();
}

bool DatabaseConnection::registerNewUser(const QString &firstName, const QString &lastName, const QString &email, const QString &password)
{
    QSqlQuery query(besMesDatabase);
    // Проверка на занятость почты
    query.prepare("SELECT is_email_available(:email)");
    query.bindValue(":email", email);
    query.exec();
    query.first();
    if (!query.value(0).toBool()) {
        // Почта занята
        return false;
    }
    query.clear();

    // Хэшируем пароль
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Algorithm::Keccak_512);

    query.prepare("SELECT register_new_user(:firstName, :lastName, :email, :hashed_password)");
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":email", email);
    query.bindValue(":hashed_password", QLatin1String(hashedPassword.toHex()));
    query.exec();
    query.first();
    return query.value(0).toBool();
}

QVector<User> DatabaseConnection::getListOfUsers()
{
    QSqlQuery query(besMesDatabase);
    // Получаем количество пользователей
    query.prepare("SELECT get_number_of_users()");
    query.exec();
    query.first();
    QVector<User> users;
    users.reserve(query.value(0).toLongLong());
    query.clear();

    query.prepare("SELECT * FROM get_list_of_users()");
    query.exec();

    while (query.next())
    {
        User currentUser{
            query.value(0).toLongLong(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString()};
        users.append(currentUser);
    }
    return users;
}

bool DatabaseConnection::createNewChat(qint64 ownerId, const QString &chatTitle)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("CALL create_chat(:owner_id, :chatTitle)");
    query.bindValue(":owernId", ownerId);
    query.bindValue(":chatTitle", chatTitle);
    query.exec();
    return true;
}

bool DatabaseConnection::deleteChat(qint64 chatId)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("CALL delete_chat(:chatId)");
    query.bindValue(":chatId", chatId);
    query.exec();
    return true;
}

QVector<Chat> DatabaseConnection::getListOfChats(qint64 userId)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT get_chats_quantity(:userId)");
    query.bindValue(":userId", userId);
    query.exec();
    query.first();
    QVector<Chat> chats;
    chats.reserve(query.value(0).toULongLong());

    query.clear();
    query.prepare("SELECT * FROM get_list_of_chats(:userId)");
    query.bindValue(":userId", userId);
    query.exec();
    while(query.next())
    {
        chats.append(Chat{query.value(0).toLongLong(), query.value(1).toString()});
    }
    return chats;
}

bool DatabaseConnection::inviteTochat(qint64 chatId, qint64 userId)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT invite_to_chat(:chatId, :userId)");
    query.bindValue(":chatId", chatId);
    query.bindValue(":userId", userId);
    query.exec();
    query.first();
    return query.value(0).toBool();
}

bool DatabaseConnection::kickFromChat(qint64 chatId, qint64 userId)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT kick_from_chat(:chatId, :userId)");
    query.bindValue(":chatId", chatId);
    query.bindValue(":userId", userId);
    query.exec();
    query.first();
    return query.value(0).toBool();
}

QVector<qint64> DatabaseConnection::getUsersInChat(qint64 chatId)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT * FROM get_users_in_chat(:chatId)");
    query.bindValue(":chatId", chatId);
    query.exec();
    QVector<qint64> users;
    while (query.next())
    {
        users.append(query.value(0).toULongLong());
    }
    return users;
}

bool DatabaseConnection::sendMessage(qint64 chatId, const QString &messageBody, qint64 senderId)
{
    QSqlQuery query(besMesDatabase);
    query.prepare("SELECT send_message(:chatId, :messageBody, :senderId)");
    query.bindValue(":chatId", chatId);
    query.bindValue(":messageBody", messageBody);
    query.bindValue(":senderId", senderId);
    query.exec();
    query.first();
    return query.value(0).toBool();
}

bool DatabaseConnection::isActive()
{
    return besMesDatabase.isOpen();
}
