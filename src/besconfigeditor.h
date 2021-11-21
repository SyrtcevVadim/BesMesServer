#ifndef BESCONFIGEDITOR_H
#define BESCONFIGEDITOR_H
// Автор: Сырцев Вадим Игоревич
#include "configeditor.h"
#include<QJsonValue>

class BesConfigEditor : public ConfigEditor
{
public:
    BesConfigEditor(const QString &configFileName);
    /// Создаёт пустой файл конфигурации сервера. В качестве аргумента передаётся
    /// имя создаваемого файла конфигурации.
    /// Важно: если такой файл уже существует, он не будет пересоздан
    static void createEmptyServerConfig(const QString &fileName);
    /// Создаёт пустой файл конфигурации базы данных. В качестве аргумента передаётся
    /// имя создаваемого файла конфигурации.
    /// Важно: если такой файл уже существует, он не будет пересоздан
    static void createEmptyDatabaseConfig(const QString &fileName);
private:
    /// Создаёт "пустой" json-объект, содержащий ключи, записанные в списке keys.
    /// Значение, устанавливаемое в соответствие ключам определяется аргументом defaultValue.
    static QJsonObject makeEmptyObject(const QStringList &keys,
                                       const QJsonValue &defaultValue="-1");
    /// Записывает объект jsonObject в файл под именем fileName
    static void writeToFile(const QJsonDocument &jsonDocument, const QString &fileName);
    /// Проверяет, существует ли в папке с файлами конфигурации файл с
    /// указанным именем
    static bool exists(const QString &fileName);

};

#endif // BESCONFIGEDITOR_H
