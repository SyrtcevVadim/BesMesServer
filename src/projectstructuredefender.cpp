#include <QDir>
#include <QFile>
#include "beslogsystem.h"
#include "besconfigreader.h"
#include "projectstructuredefender.h"
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
     * -logs
     * -configs
     * -certificate
     */
    QDir currentDir;
    currentDir.mkdir(LOG_DIR_NAME);
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
