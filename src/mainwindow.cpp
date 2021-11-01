#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "multithreadtcpserver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new MultithreadTcpServer(QHostAddress::Any, 1234);
    configureViews();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configureViews()
{
    // Запускает работу сервера
    connect(ui->startServerBtn, SIGNAL(clicked()), server, SLOT(start()));
    // Останавливает работу сервера
    connect(ui->stopServerBtn, SIGNAL(clicked()), server, SLOT(stop()));

    // Когда количество активных подключений изменяется, обновляем счётчик в GUI
    connect(server, SIGNAL(activeConnectionsChanged(int)),
            SLOT(setActiveConnectionsCounter(int)));
}

void MainWindow::setActiveConnectionsCounter(int counter)
{
    ui->activeConnectionsCounter->setText(QString("%1").setNum(counter));
}
