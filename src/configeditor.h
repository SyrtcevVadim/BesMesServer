#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H
// Автор: Сырцев Вадим Игоревич
#include<QString>
#include<QFile>
#include<QMap>
#include<QVariant>

#define STANDART_CONFIG_DIR_NAME QString("configs")

class ConfigEditor
{
public:
    /// Создаёт редактор конфигурационного файла. В параметрах указывается имя файла и директория,
    /// в которой эти файлы содержатся
    ConfigEditor(const QString &configFileName);
    /// Возвращает строку значение по ключу.
    /// Если предоставленного ключа не существует, возвращается пустая строка
    QString getString(const QString &key);
    /// Возвращает числовое значение по ключу.
    /// Если предоставленного ключа не существует, возвращается -1
    int getInt(const QString &key);
    /// Возвращает ассоциативный массив, где ключи и значения - строки
    /// QVariant
    /// Если предоставленного ключа не существует, возвращается пустой ассоциативный массив
    QMap<QString, QString> getMap(const QString &key);
    /// Возвращает список строк.
    /// Если предоставленного ключа не существует, возвращает пустой список
    QStringList getStringList(const QString &key);
    /// Устанавливает для ключа key значение value. Если такого ключа не существует, ничего не
    /// проиходит
    void setValue(const QString &key, const QVariant &value);
    /// Сохраняет параметры конфигурации в файле
    void saveToFile();
    /// Считывает параметры из файла конфигурации
    void retrieveParameters();


    /// Создаёт директорию для конфигурационных файлов. В качетстве аргумента можно указать
    /// имя создаваемой директории. Имя директории указывается с помощью функции setConfigDirectoryName
    static void createConfigDirectory();
    /// Устанавливает имя директории с файлами конфигурации, в которой следует искать файлы конфигурации
    static void setConfigDirectoryName(const QString &path);
protected:
    /// Путь к конфигурационному файлу, с которым связан этот объект
    QString pathToConfigFile;
    /// Ассоциативный массив параметров, полученных из файла конфигурации
    QMap<QString, QVariant> parameters;

    /// Путь к директории с файлами конфигурации. По умолчанию
    /// папка с конфигурациями определяется макросом STANDART_CONFIG_DIR_NAME
    static QString pathToConfigDirectory;

};

#endif // CONFIGEDITOR_H
