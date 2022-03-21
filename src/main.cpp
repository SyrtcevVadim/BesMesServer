#include <QtWidgets>
#include "mainwindow.h"
#include "projectstructuredefender.h"
#include "clientconnection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Все строки хранятся в кодировке UTF-8
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    ProjectStructureDefender::init();
    ClientConnection::initSslConfiguration();

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
