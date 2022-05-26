#ifndef PROJECT_STRUCTURE_DEFENDER_H
#define PROJECT_STRUCTURE_DEFENDER_H

#define CONFIG_FILE_NAME QString("configs.toml")

class ProjectStructureDefender
{
public:
    /// Создаёт необходимые файлы проекта, т.к. они могут быть умышленно или случайно
    /// удалены пользователем
    static void init();

private:
    /// Инициализирует директории проекта. Эта функция должна
    /// вызываться в самом начале работы программы, т.к. пользователь
    /// может случайно или умышленно повредить файлы системы
    static void initDirectories();
    /// Если конфигурационный файл не существует, создаёт его и вставляет туда пустые настройки
    /// Если такой файл существует, нечего не делает
    static void initConfigFile();
};

#endif // PROJECT_STRUCTURE_DEFENDER_H
