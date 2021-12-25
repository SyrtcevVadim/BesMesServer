#ifndef PROJECTSTRUCTUREDEFENDER_H
#define PROJECTSTRUCTUREDEFENDER_H


class ProjectStructureDefender
{
public:
    /// Инициализирует директории проекта. Эта функция должна
    /// вызываться в самом начале работы программы, т.к. пользователь
    /// может случайно или умышленно повредить файлы системы
    static void init();
};

#endif // PROJECTSTRUCTUREDEFENDER_H
