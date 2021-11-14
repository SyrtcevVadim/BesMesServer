#ifndef CONFIGFILEEDITOR_H
#define CONFIGFILEEDITOR_H

#include<QFile>
#include<QTextStream>
#include<QHash>

#define STANDART_CONFIG_FILE_NAME "config.txt"
#define MIN_SETTINGS_NUMBER 4
#define STANDART_PARAMETER_VALUE "-1"

class ConfigFileEditor
{
public:
    /// При создании объект начинает сразу считывать данные из файла
    /// конфигурации. Поэтому после создания сразу можно получать необходимые
    /// данные
    ConfigFileEditor(const QString &configFileName=STANDART_CONFIG_FILE_NAME);
    /// По названию параметра возвращает его значение из конфигурационного файла
    QString getParameterValue(const QString &parameterName);
    /// Устанавливае соответствующему параметру новое значение в файле конфигурации
    void setParameterValue(const QString &parameterName, const QString &parameterValue);
    /// Проверяет, установлено ли значение параметров. True, если установлено.
    /// Иначе, false. Параметр считается установленым, если его значение отличается от -1
    bool areParametersSet();
    /// Перезаписывает файл конфигурации
    void updateConfigFile();
    /// Используется для доступа к значениям параметров
    QString& operator[] (const QString &key);

private:
    /// Создаёт пустой файл конфигурации со значениями, определяемыми
    /// макросом STANDART_PARAMETER_VALUE
    void createEmptyConfigFile(const QString &configFileName);

    /// Путь к файлу конфигурации
    QString configFileName;
    /// Параметры, сохранённые в файл конфигурации
    QHash<QString, QString> parameters;
    /// Список имён параметров, которые должны находиться в файле конфигурации
    static QStringList necessaryParameterNames;
};

#endif // CONFIGFILEEDITOR_H
