#include "configeditor.h"
#include<QDir>
#include<QByteArray>
#include<QJsonDocument>
#include<QJsonObject>



QString ConfigEditor::pathToConfigDirectory{QDir::currentPath()+"/"+STANDART_CONFIG_DIR};

ConfigEditor::ConfigEditor(const QString &configFileName)
{
    QFile configFile(pathToConfigDirectory+"/"+configFileName);
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

void ConfigEditor::createConfigDirectory(const QString &configDirName)
{
    QDir currentDirectory = QDir::currentPath();
    currentDirectory.mkdir(configDirName);
}
