#include <QDir>
#include "beslogsystem.h"
#include "besconfigeditor.h"
#include "projectstructuredefender.h"

void ProjectStructureDefender::init()
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
