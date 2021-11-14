#include<QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "multithreadtcpserver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    configureServer(&configParameters);
    configureViews();

    // Отображаем в UI параметры конфигурации, записанные в файле
    showConfigParameters();

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


    connect(ui->saveConfigParametersBtn, SIGNAL(clicked()), SLOT(saveConfigParameters()));
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

void MainWindow::configureServer(ConfigFileEditor *configParameters)
{
    // Создаём многопоточный сервер
    server = new MultithreadTcpServer(QHostAddress::Any, 1234, configParameters);
    // Как только сервер запустился, это будет отображено в UI
    connect(server, SIGNAL(started()), SLOT(showServerStateAsActive()));
    connect(server, SIGNAL(stopped()), SLOT(showServerStateAsPassive()));
}

void MainWindow::showConfigParameters()
{
    ui->databaseAddressEdit->setText(configParameters["database_address"]);
    ui->databasePortEdit->setText(configParameters["database_port"]);
    ui->databaseUserNameEdit->setText(configParameters["user_name"]);
    // Пароль не отображаем в целях безопасности :)
}

void MainWindow::saveConfigParameters()
{
    qDebug() << "Сохраняем данные в конфигурационный файл";
    // Нельзя допустить сохранения пустых значений параметров
    QString databaseAddress = ui->databaseAddressEdit->text();
    QString databasePort = ui->databasePortEdit->text();
    QString userName = ui->databaseUserNameEdit->text();
    QString password = ui->databaseUserPasswordEdit->text();
    // TODO Если какое-либо поле пустое, нужно заблокировать кнопку сохранения результатов!
    if(databaseAddress.isEmpty() || databasePort.isEmpty() ||
        userName.isEmpty())
    {
        QMessageBox::critical(this, tr("Ошибка изменения параметров конфигурации"),
                              tr("Нельзя оставлять поле ввода параметра пустым(кроме пароля)"), QMessageBox::Ok);
    }
    else
    {
        configParameters["database_address"]=databaseAddress;
        configParameters["database_port"]=databasePort;
        configParameters["user_name"]=userName;
        if(!password.isEmpty())
        {
            configParameters["password"]=password;
        }
        // Сохраняем данные в файл конфигурации
        configParameters.updateConfigFile();
        emit configParametersChanged();
    }
}

