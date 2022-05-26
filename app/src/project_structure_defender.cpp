#include <QDir>
#include <QFile>
#include "config_reader.h"
#include "project_structure_defender.h"
#include "libs/include/toml.hpp"

void ProjectStructureDefender::init()
{
    initDirectories();
    initConfigFile();
}

void ProjectStructureDefender::initDirectories()
{
    /*
     * Проект имеет (сырую. TODO) следующую древовидную струтуру:
     * -configs
     * -certificate
     */
    QDir currentDir;
    currentDir.mkdir(CONFIG_DIR_NAME);
    currentDir.mkdir("certificate");
}

void ProjectStructureDefender::initConfigFile()
{
    QFile configFile{CONFIG_DIR_NAME+"/"+CONFIG_FILE_NAME};
    if(!configFile.exists())
    {
        QFile configsTemplateFile(":/templates/configs.toml");
        // Копируем пустой шаблон configs.toml в директорию с файлами конфигурации
        configsTemplateFile.copy(QString("%1/configs.toml").arg(CONFIG_DIR_NAME));
    }
}
