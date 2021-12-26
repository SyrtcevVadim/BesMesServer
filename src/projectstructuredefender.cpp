#include <QDir>
#include <QFile>
#include "beslogsystem.h"
#include "besconfigeditor.h"
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
     */

    /*
     * Этот блок кода предназначен для обеспечения целостности конфигурационных файлов.
     * При первом запуске программы создаются все необходимые файлы. Администратору программы
     * остаётся лишь заполнить.
     * Создаём директорию для конфигурационных файлов
     */

    BesConfigEditor::setConfigDirectoryName(STANDART_CONFIG_DIR_NAME);
    BesConfigEditor::createConfigDirectory();

    BesConfigEditor::createEmptyDatabaseConnectionConfig(DATABASE_CONFIG_FILE_NAME);
    BesConfigEditor::createEmptyServerConfig(SERVER_CONFIG_FILE_NAME);
    BesConfigEditor::createEmptyEmailSenderConfig(EMAIL_SENDER_CONFIG_FILE_NAME);

    BesLogSystem::createLogsDirectory();
}

void ProjectStructureDefender::initConfigFile()
{
    QFile configFile{STANDART_CONFIG_DIR_NAME+"/"+CONFIG_FILE_NAME};
    if(!configFile.exists())
    {
        QFile configsTemplateFile(":/templates/configs.toml");
        // Копируем пустой шаблон configs.toml в директорию с файлами конфигурации
        configsTemplateFile.copy(QString("%1/configs.toml").arg(STANDART_CONFIG_DIR_NAME));
    }
}
