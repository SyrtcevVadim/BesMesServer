#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "multithreadtcpserver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Создаём многопоточный сервер
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
    connect(server, SIGNAL(activeConnectionsCounterChanged(unsigned long long)),
            SLOT(setActiveConnectionsCounter(unsigned long long)));
}

void MainWindow::setActiveConnectionsCounter(unsigned long long counter)
{
    // Обновляем счётчик активных подключений
    ui->activeConnectionsCounter->setText(QString("%1").setNum(counter));
}
