#include "configeditor.h"
#include<QDir>
#include<QByteArray>
#include<QJsonDocument>
#include<QJsonObject>



QString ConfigEditor::pathToConfigDirectory;

ConfigEditor::ConfigEditor(const QString &configFileName)
{
    pathToConfigFile = pathToConfigDirectory+"/"+configFileName;
    retrieveParameters();
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

QMap<QString, QString> ConfigEditor::getMap(const QString &key)
{
    if(parameters.contains(key))
    {
        QMap<QString, QVariant> initial = parameters[key].toMap();
        QMap<QString, QString> result;
        for(const QString &k: initial.keys())
        {
            result[k] = initial[k].toString();
        }
        return result;
    }
    return QMap<QString, QString>();
}

QStringList ConfigEditor::getStringList(const QString &key)
{
    if(parameters.contains(key))
    {
        QList<QVariant> initial = parameters[key].toList();
        QStringList result;
        for(const QVariant &value: initial)
        {
            result.push_back(value.toString());
        }
        return result;
    }
    return QStringList();
}



void ConfigEditor::setValue(const QString &key, const QVariant &value)
{
    if(parameters.contains(key))
    {
        parameters[key] = value;
    }
}


void ConfigEditor::saveToFile()
{
    // Подготавливаем документ для записи в файл
    QJsonDocument document = QJsonDocument::fromVariant(parameters);
    QFile configFile(pathToConfigFile);
    configFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    configFile.write(document.toJson(QJsonDocument::Indented));
    configFile.close();
}

void ConfigEditor::retrieveParameters()
{
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

void ConfigEditor::createConfigDirectory()
{
    QDir currentDirectory;
    currentDirectory.mkdir(pathToConfigDirectory);
}

void ConfigEditor::setConfigDirectoryName(const QString &path)
{
    pathToConfigDirectory=path;
}
