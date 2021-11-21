#include "besconfigeditor.h"
#include<QJsonObject>
#include<QJsonDocument>
#include<QTextStream>
#include<QFileInfo>

BesConfigEditor::BesConfigEditor(const QString &configFileName):
    ConfigEditor(configFileName)
{}

bool BesConfigEditor::exists(const QString &fileName)
{
    // Сначала проверяем, не создан ли еще файл конфигурации с таким названием
    return QFileInfo::exists(pathToConfigDirectory+"/"+fileName);
}

void BesConfigEditor::createEmptyDatabaseConfig(const QString &fileName)
{
    // Если файл конфигурации с таким именем существует, не пересоздаём его
    if(exists(fileName))
    {
        return;
    }
    /*
     * В файле конфигурации подключения к базе данных хранятся следующие данные:
     * address - ip-адрес базы данных
     * port - порт, прослушиваемый базой данных
     * userName - имя аккаунта, под которым рабочий поток будет подключаться к серверу
     * password - пароль от аккаунта
     */
    QJsonDocument document(makeEmptyObject({"address", "port", "userName", "password"}));

    // Создаём файл конфигурации
    writeToFile(document, fileName);
}

void BesConfigEditor::createEmptyServerConfig(const QString &fileName)
{
    // Если файл конфигурации с таким именем существует, не пересоздаём его
    if(exists(fileName))
    {
        return;
    }
    /*
     * В файле конфигурации сервера хранятся следующие данные:
     * port - прослушиваемый сервером порт
     */
    // Создаём конфигурационный json-файл
    QJsonDocument document(makeEmptyObject({"port"}));
    // Сохраняем его в файловой системе под именем fileName
    writeToFile(document, fileName);

}

QJsonObject BesConfigEditor::makeEmptyObject(const QStringList &keys, const QJsonValue &defaultValue)
{
    QJsonObject newObject;
    // Объект называется пустым, потому что значение каждого из ключа - недействительное значение,
    // которое должно быть заменено чем-нибудь осмысленным
    for(const QString &key: keys)
    {
        newObject[key] = defaultValue;
    }
    return newObject;
}

void BesConfigEditor::writeToFile(const QJsonDocument &jsonDocument, const QString &fileName)
{
    QFile newConfigFile(pathToConfigDirectory+"/"+fileName);
    newConfigFile.open(QIODevice::WriteOnly);
    newConfigFile.write(jsonDocument.toJson(QJsonDocument::Indented));
    newConfigFile.close();
}
