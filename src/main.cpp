#include <QtWidgets>
#include "mainwindow.h"
#include "projectstructuredefender.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ProjectStructureDefender::init();

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
