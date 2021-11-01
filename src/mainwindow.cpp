#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "multithreadtcpserver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new MultithreadTcpServer(QHostAddress::Any, 1234);
    server->start();
    qDebug() << "Сервер начинает прослушивать входящие соединения";
}

MainWindow::~MainWindow()
{
    delete ui;
}
