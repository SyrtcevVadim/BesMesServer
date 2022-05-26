#include<string_view>
#include<QDebug>
#include "config_reader.h"
#include "singleton_base.h"

ConfigReader::ConfigReader(const QString &configureDirectoryName, const QString &configureFileName)
    : SingletonBase()
{

    readConfigs(configureDirectoryName, configureFileName);
}

QString ConfigReader::getString(const QString &table, const QString &name)
{
    std::optional<std::string> result = configs[table.toStdString()][name.toStdString()].value_or<std::string>("Не установленное значение");
    std::string resultStr = result.value();
    return QString(resultStr.c_str());
}

quint16 ConfigReader::getQuint16(const QString &table, const QString &name)
{
    std::optional<quint16> result = configs[table.toStdString()][name.toStdString()].value_or<quint16>(0);
    return result.value();
}

void ConfigReader::readConfigs(const QString &configureDirectoryName, const QString &configureFileName)
{
    std::string configFileName{QString("%1/%2").arg(configureDirectoryName, configureFileName).toStdString()};
    // Считываем конфигурационный файл в таблицу
    try
    {
        configs = toml::parse_file(configFileName);
    }
    catch(toml::parse_error error)
    {
        qDebug() << "Не удалось открыть конфигурационный файл";
    }
}

quint16 ConfigReader::getServerListeningPort()
{
    return getQuint16("server", "listening_port");
}

QString ConfigReader::getPathToPrivateKey()
{
    return getString("security","path_to_private_key");
}

QString ConfigReader::getPathToCertificate()
{
    return getString("security", "path_to_certificate");
}

QString ConfigReader::getPathPhrase()
{
    return getString("security", "pass_phrase");
}

QString ConfigReader::getDatabaseName()
{
    return getString("database", "name");
}

QString ConfigReader::getDatabaseHostAddress()
{
    return getString("database", "host_address");
}

quint16 ConfigReader::getDatabaseListeningPort()
{
    return getQuint16("database","listening_port");
}

QString ConfigReader::getDatabaseUserName()
{
    return getString("database_connection", "user_name");
}

QString ConfigReader::getDatabaseUserPassword()
{
    return getString("database_connection", "password");
}
