#ifndef BESCONFIGEDITOR_H
#define BESCONFIGEDITOR_H
// Автор: Сырцев Вадим Игоревич
#include<QString>
#include "mutex"
#include "libs/include/toml.hpp"

#define CONFIG_DIR_NAME QString("configs")
#define CONFIG_FILE_NAME QString("configs.toml")

using std::mutex;
using std::lock_guard;

/// Класс-обёртка для работы с файлами конфигурации
/// ВАЖНО: реализует паттерн "Одиночка"
class BesConfigReader
{
public:
    BesConfigReader(BesConfigReader &)=delete;
    void operator=(const BesConfigReader &)=delete;
    /// Возвращает строковое значение параметра с названием name из таблицы(секции) table
    /// Допустим, у нас есть конфигурационный файл со следующим содержимым:
    /// [database]
    /// address = "127.0.0.1"
    /// Получается, database - имя таблицы, address - имя поля
    QString getString(const QString &table, const QString &name);
    /// Возвращает целочисленное значение параметра name
    int getInt(const QString &table, const QString &name);
    /// Читает из конфигурационного файла настройки
    void readConfigs();
    /// Возвращает указатель на объект
    static BesConfigReader* getInstance();
private:
    static BesConfigReader *_instance;
    static mutex _mutex;
protected:
    BesConfigReader();
    ~BesConfigReader();

    /// В этой таблице будут хранится настройки из файла конфигурации
    toml::table configs;
};

#endif // BESCONFIGEDITOR_H
