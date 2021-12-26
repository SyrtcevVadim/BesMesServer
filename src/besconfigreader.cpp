#include "besconfigreader.h"
#include<QDebug>
#include<string_view>

BesConfigReader* BesConfigReader::_instance{nullptr};
mutex BesConfigReader::_mutex;

BesConfigReader::BesConfigReader()
{
    readConfigs();
}


BesConfigReader::~BesConfigReader(){}


BesConfigReader* BesConfigReader::getInstance()
{
    // Блокируем доступ к объекту для остальных вызывающих потоков
    lock_guard<mutex> lock(_mutex);
    if(_instance == nullptr)
    {
        _instance = new BesConfigReader();
    }
    return _instance;
}

QString BesConfigReader::getString(const QString &table, const QString &name)
{
    std::optional<std::string> result = configs[table.toStdString()][name.toStdString()].value_or<std::string>("abcd");
    std::string resultStr = result.value();
    return QString(resultStr.c_str());
}

int BesConfigReader::getInt(const QString &table, const QString &name)
{
    std::optional<int> result = configs[table.toStdString()][name.toStdString()].value_or<int>(0);
    return result.value();
}

void BesConfigReader::readConfigs()
{
    std::string_view configFileName{QString("%1/%2")
                                   .arg(CONFIG_DIR_NAME, CONFIG_FILE_NAME).toStdString()};
    // Считываем конфигурационный файл в таблицу
    configs = toml::parse_file(configFileName);
}
