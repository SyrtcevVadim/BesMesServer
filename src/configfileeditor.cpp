#include "configfileeditor.h"
#include<QFileInfo>
#include <QDebug>
#include <QMessageBox>

QStringList ConfigFileEditor::necessaryParameterNames{"database_address",
                                                      "database_port",
                                                      "user_name",
                                                      "password"};

ConfigFileEditor::ConfigFileEditor(const QString &configFileName)
{
    this->configFileName = configFileName;
    QFileInfo configFileInfo(configFileName);
    if(configFileInfo.exists())
    {
        qDebug() << "Файл конфигов существует!";
        // Готовимся считывать данные из файла конфигурации
        QFile configFile(configFileName);
        configFile.open(QIODevice::ReadOnly);
        QTextStream configStream(&configFile);

        /* Если установить действительный размер массива заранее,
         * то большинство его операций станут асимптотически более
         * эффективными.
         * Мы ожидаем, что в файле конфигурации будет следующие параметры:
         * database_address, database_port, user_name, password
         */
        parameters.reserve(MIN_SETTINGS_NUMBER);
        // Считываем данные из файла конфигурации
        QStringList configSettings = configStream.readAll().split("\r\n");
        for(QString settingString: configSettings)
        {
            // Защита на случай, если конфигурационный файл содержит пустые строки
            if(!settingString.isEmpty())
            {
               QStringList args = settingString.split("=");
               QString key = args[0], value = args[1];
               parameters.insert(key, value);
            }
        }
        qDebug() << parameters;
    }
    else
    {
        qDebug() << "Создаём файл конфигурации";
        createEmptyConfigFile(configFileName);
    }
    if(!areParametersSet())
    {
        QMessageBox::critical(nullptr,
                              QObject::tr("Пустой файл конфигурации"),
                              QString(QObject::tr("Не заданы параметры в файле \"%1\"!")).arg(configFileName),
                              QMessageBox::Abort);
        throw "Config file has been created but there're empty values";
    }
}

void ConfigFileEditor::updateConfigFile()
{
    QFile configFile(configFileName);
    configFile.open(QIODevice::Truncate|QIODevice::WriteOnly);
    QTextStream configStream(&configFile);
    for(QString parameterName: parameters.keys())
    {
        configStream << parameterName << "="<<parameters[parameterName]<<"\r\n";
    }
    configStream.flush();
}

void ConfigFileEditor::setParameterValue(const QString &parameterName, const QString &parameterValue)
{
    if(parameters.contains(parameterName))
    {
        parameters[parameterName]=parameterValue;
        updateConfigFile();
    }
    else
    {
        qDebug() << "Параметра "<<parameterName <<" не существует!";
    }
}

QString ConfigFileEditor::getParameterValue(const QString &parameterName)
{
    if(parameters.contains(parameterName))
    {
        return parameters[parameterName];
    }
    return "";
}

void ConfigFileEditor::createEmptyConfigFile(const QString &configFileName)
{
    QFile newConfigFile(configFileName);
    newConfigFile.open(QIODevice::WriteOnly);
    QTextStream configStream(&newConfigFile);
    // Для каждого параметра устанавливаем стандартное значение
    for(QString parameterName: necessaryParameterNames)
    {
        configStream<<parameterName<<"="<<STANDART_PARAMETER_VALUE<<"\r\n";
    }
    configStream.flush();
}

bool ConfigFileEditor::areParametersSet()
{
    for(QString parameterValue: parameters.values())
    {
        /* Если хотя бы один параметр не установлен, мы не сможем
         * использовать данный файл конфигурации
         */
        if(parameterValue == STANDART_PARAMETER_VALUE)
        {
            return false;
        }
    }
    return true;
}


QString& ConfigFileEditor::operator[](const QString &key)
{
    return parameters[key];
}
