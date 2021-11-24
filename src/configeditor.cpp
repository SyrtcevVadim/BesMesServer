#include "configeditor.h"
#include<QDir>
#include<QByteArray>
#include<QJsonDocument>
#include<QJsonObject>



QString ConfigEditor::pathToConfigDirectory{QDir::currentPath()+"/"+STANDART_CONFIG_DIR};

ConfigEditor::ConfigEditor(const QString &configFileName)
{
    pathToConfigFile = pathToConfigDirectory+"/"+configFileName;
    QFile configFile(pathToConfigFile);
    configFile.open(QIODevice::ReadOnly);
    // Считываем "сырые" данные из файла конфигурации
    QByteArray configData = configFile.readAll();
    configFile.close();
    // Интерпретируем эти данные, как документ в формате json
    QJsonDocument document = QJsonDocument::fromJson(configData);
    // Получаем ассоциативный массив из данных файла
    parameters = (document.object()).toVariantMap();
}

QString ConfigEditor::getString(const QString &key)
{
    if(parameters.contains(key))
    {
        return parameters[key].toString();
    }
    return "";
}

int ConfigEditor::getInt(const QString &key)
{
    if(parameters.contains(key))
    {
        return parameters[key].toInt();
    }
    return -1;
}

QMap<QString, QVariant> ConfigEditor::getMap(const QString &key)
{
    if(parameters.contains(key))
    {
        return parameters[key].toMap();
    }
    return QMap<QString, QVariant>();
}

void ConfigEditor::createConfigDirectory(const QString &configDirName)
{
    QDir currentDirectory = QDir::currentPath();
    currentDirectory.mkdir(configDirName);
}

void ConfigEditor::setValue(const QString &key, const QVariant &value)
{
    if(parameters.contains(key))
    {
        parameters[key] = value;
    }
}


void ConfigEditor::updateConfigFile()
{
    // Подготавливаем документ для записи в файл
    QJsonDocument document = QJsonDocument::fromVariant(parameters);
    QFile configFile(pathToConfigFile);
    configFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    configFile.write(document.toJson(QJsonDocument::Indented));
    configFile.close();
}
