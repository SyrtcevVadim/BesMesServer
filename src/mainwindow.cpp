#include<QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "multithreadtcpserver.h"
#include "besconfigreader.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    configureServer();
    configureViews();

    // Отображаем в UI параметры конфигурации, записанные в файле
    showConfigParameters();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
}


void MainWindow::updateServerWorkingTimeCounter(QString time)
{
    ui->currentSessionWorkingTimeLbl->setText(time);
}

void MainWindow::configureViews()
{
    // Запускает работу сервера
    connect(ui->startServerBtn, SIGNAL(clicked()), server, SLOT(start()));
    connect(ui->startServerBtn, SIGNAL(clicked()), SLOT(toggleStartStopBtns()));
    // Останавливает работу сервера
    connect(ui->stopServerBtn, SIGNAL(clicked()), server, SLOT(stop()));
    connect(ui->stopServerBtn, SIGNAL(clicked()), SLOT(toggleStartStopBtns()));

    // Когда количество активных подключений изменяется, обновляем счётчик в GUI
    connect(server, SIGNAL(activeConnectionsCounterChanged(unsigned long long)),
            SLOT(setActiveConnectionsCounter(unsigned long long)));

}

void MainWindow::setActiveConnectionsCounter(unsigned long long counter)
{
    // Обновляем счётчик активных подключений
    ui->activeConnectionsCounter->setText(QString("%1").setNum(counter));
}

void MainWindow::showServerStateAsActive()
{
    ui->currentServerStateLbl->setText(ACTIVE_SERVER_STATE_TEXT);
}

void MainWindow::showServerStateAsPassive()
{
    ui->currentServerStateLbl->setText(PASSIVE_SERVER_STATE_TEXT);
}

void MainWindow::configureServer()
{
    // Создаём многопоточный сервер
    server = new MultithreadTcpServer(QHostAddress::Any);

    // Как только сервер запустился, это будет отображено в UI
    connect(server, SIGNAL(started()), SLOT(showServerStateAsActive()));
    connect(server, SIGNAL(stopped()), SLOT(showServerStateAsPassive()));

    connect(server, SIGNAL(workingTimeUpdated(QString)),
            SLOT(updateServerWorkingTimeCounter(QString)));
}

void MainWindow::showConfigParameters()
{
    BesConfigReader *configs = BesConfigReader::getInstance();
    ui->databaseAddressEdit->setText(configs->getString("database", "address"));
    ui->databasePortEdit->setText(QString().setNum(configs->getInt("database", "port")));
    ui->databaseUserNameEdit->setText(configs->getString("database_connection", "user_name"));
    // Пароль не отображаем в целях безопасности :)
}

void MainWindow::toggleStartStopBtns()
{
    // Изначально кнопка запуска сервера активна и нажать могут только
    // на неё
    static bool startBtnEnabled=true;
    if(startBtnEnabled)
    {
        // Блокируем кнопку запуска сервера и активируем кнопку остановки
        ui->startServerBtn->setEnabled(false);
        ui->stopServerBtn->setEnabled(true);
    }
    else
    {
        // Блокируем кнопку остановки и активируем кнопку запуска
        ui->stopServerBtn->setEnabled(false);
        ui->startServerBtn->setEnabled(true);
    }
    startBtnEnabled=ui->startServerBtn->isEnabled();
}


