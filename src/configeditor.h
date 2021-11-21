#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H
// Автор: Сырцев Вадим Игоревич
#include<QString>
#include<QFile>
#include<QMap>
#include<QVariant>

#define STANDART_CONFIG_DIR QString("configs")

class ConfigEditor
{
public:
    /// Создаёт редактор конфигурационного файла. В параметрах указывается имя файла и директория,
    /// в которой эти файлы содержатся
    ConfigEditor(const QString &configFileName);
    /// Возвращает строку-значение по ключу.
    /// Если предоставленного ключа не существует, возвращается пустая строка
    QString getString(const QString &key);
    /// Создаёт директорию для конфигурационных файлов. В качетстве аргумента можно указать
    /// имя создаваемой директории. По умолчанию оно определено макросом STANDART_CONFIG_DIR
    static void createConfigDirectory(const QString &configDirName=STANDART_CONFIG_DIR);
protected:
    /// Ассоциативный массив параметров, полученных из файла конфигурации
    QMap<QString, QVariant> parameters;

    /// Путь к директории с файлами конфигурации. По умолчанию
    /// папка с конфигурациями определяется макросом STANDART_CONFIG_DIR
    static QString pathToConfigDirectory;
};

#endif // CONFIGEDITOR_H
