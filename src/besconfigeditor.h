#ifndef BESCONFIGEDITOR_H
#define BESCONFIGEDITOR_H
// Автор: Сырцев Вадим Игоревич
#include "configeditor.h"
#include<QJsonValue>

#define DATABASE_CONFIG_FILE_NAME "databaseConnectionConfig.json"
#define SERVER_CONFIG_FILE_NAME "serverConfig.json"
#define EMAIL_SENDER_CONFIG_FILE_NAME "emailSenderConfig.json"

class BesConfigEditor : public ConfigEditor
{
public:
    BesConfigEditor(const QString &configFileName);
    /// Создаёт пустой файл конфигурации сервера. В качестве аргумента передаётся
    /// имя создаваемого файла конфигурации.
    /// Важно: если такой файл уже существует, он не будет пересоздан
    static void createEmptyServerConfig(const QString &fileName);
    /// Создаёт пустой файл конфигурации подключения к  базе данных. В качестве аргумента передаётся
    /// имя создаваемого файла конфигурации.
    /// Важно: если такой файл уже существует, он не будет пересоздан
    static void createEmptyDatabaseConnectionConfig(const QString &fileName);
    /// Создаёт пустой файл конфигурации для системы отправки писем на электронную почту
    /// Важно: если такой файл уже существует, он не будет пересоздан
    static void createEmptyEmailSenderConfig(const QString &fileName);
private:
    /// Записывает объект jsonObject в файл под именем fileName
    static void writeToFile(const QJsonDocument &jsonDocument, const QString &fileName);
    /// Проверяет, существует ли в папке с файлами конфигурации файл с
    /// указанным именем
    static bool exists(const QString &fileName);

};

#endif // BESCONFIGEDITOR_H
