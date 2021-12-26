#include <QtWidgets>
#include "mainwindow.h"
#include "projectstructuredefender.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Все строки хранятся в кодировке UTF-8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    ProjectStructureDefender::init();

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
