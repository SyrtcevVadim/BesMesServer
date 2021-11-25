#include "besconfigeditor.h"
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonArray>
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

void BesConfigEditor::createEmptyDatabaseConnectionConfig(const QString &fileName)
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


    QJsonObject newObject;
    // Объект называется пустым, потому что значение каждого из ключа - недействительное значение,
    // которое должно быть заменено чем-нибудь осмысленным
    newObject["address"]=STANDART_STRING;
    newObject["port"]=STANDART_NUMBER;
    newObject["userName"]=STANDART_STRING;
    newObject["password"]=STANDART_STRING;
    newObject["databaseName"]=STANDART_STRING;

    QJsonDocument document(newObject);

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

    QJsonObject newObject;
    newObject["port"]=STANDART_NUMBER;
    newObject["superUserLogin"]=STANDART_STRING;
    newObject["superUserPassword"]=STANDART_STRING;

    // Создаём конфигурационный json-файл
    QJsonDocument document(newObject);
    // Сохраняем его в файловой системе под именем fileName
    writeToFile(document, fileName);

}

void BesConfigEditor::createEmptyEmailSenderConfig(const QString &fileName)
{
    if(exists(fileName))
    {
        return;
    }
    /*
     * В файле конфигурации системы отправки писем на электронную почту хранятся следующие данные:
     * smtpServerAddress - ip-адрес smpt-сервера, который используется при отправке письма
     * smptServerPort - порт, прослушиваемый этим сервером
     *
     * senderEmails - ассоциативный массив. Ключи в нём - адреса электронных почт, используемых
     * для отправки сообщений на почты пользователей, а значения - пароли от этих почт.
     * verificationEmailBody - шаблон тела письма для отправки кода верификации регистрации
     * verificationEmailTitle - заголовок тела письма для отправки кода верификации регистрации
     */
    QJsonObject newObject;
    newObject["smtpServerAddress"]=STANDART_STRING;
    newObject["smtpServerPort"]=STANDART_NUMBER;
    newObject["senderEmails"]=STANDART_MAP;
    newObject["verificationEmailBody"]= STANDART_LIST;
    newObject["verificationEmailTitle"] =STANDART_STRING;

    QJsonDocument document(newObject);
    writeToFile(document, fileName);
}

void BesConfigEditor::writeToFile(const QJsonDocument &jsonDocument, const QString &fileName)
{
    QFile newConfigFile(pathToConfigDirectory+"/"+fileName);
    newConfigFile.open(QIODevice::WriteOnly);
    newConfigFile.write(jsonDocument.toJson(QJsonDocument::Indented));
    newConfigFile.close();
}
