#ifndef BESCONFIGEDITOR_H
#define BESCONFIGEDITOR_H
// Автор: Сырцев Вадим Игоревич
#include<QString>

#include "libs/include/toml.hpp"
#include "singleton_base.h"

#define CONFIG_DIR_NAME QString("configs")
#define CONFIG_FILE_NAME QString("configs.toml")

class IConfigReader
{
public:
    virtual quint16 getServerListeningPort() = 0;
    virtual QString getPathToPrivateKey() = 0;
    virtual QString getPathToCertificate() = 0;
    virtual QString getPathPhrase() = 0;

    virtual QString getDatabaseName() = 0;
    virtual QString getDatabaseHostAddress() = 0;
    virtual quint16 getDatabaseListeningPort() = 0;

    virtual QString getDatabaseUserName() = 0;
    virtual QString getDatabaseUserPassword() = 0;
};


/// Класс-обёртка для работы с файлами конфигурации
/// ВАЖНО: реализует паттерн "Одиночка"
class ConfigReader: public SingletonBase<ConfigReader>, public IConfigReader
{
public:
    quint16 getServerListeningPort() override;
    QString getPathToPrivateKey() override;
    QString getPathToCertificate() override;
    QString getPathPhrase() override;
    QString getDatabaseName() override;
    QString getDatabaseHostAddress() override;
    quint16 getDatabaseListeningPort() override;
    QString getDatabaseUserName() override;
    QString getDatabaseUserPassword() override;

private:
    /// Возвращает строковое значение параметра с названием name из таблицы(секции) table
    /// Допустим, у нас есть конфигурационный файл со следующим содержимым:
    /// [database]
    /// address = "127.0.0.1"
    /// Получается, database - имя таблицы, address - имя поля
    QString getString(const QString &table, const QString &name);
    /// Возвращает целочисленное значение параметра name
    /// Читает из конфигурационного файла настройки
    quint16 getQuint16(const QString &table, const QString &name);
    /// Считывает настройки системы из файла конфигурации
    void readConfigs(const QString &configureDirectoryName, const QString &configureFileName);
    ConfigReader(const QString &configureDirectoryName = CONFIG_DIR_NAME, const QString &configureFileName = CONFIG_FILE_NAME);
    /// В этой таблице будут хранится настройки из файла конфигурации
    toml::table configs;

    friend class SingletonBase<ConfigReader>;
};

#endif // BESCONFIGEDITOR_H
