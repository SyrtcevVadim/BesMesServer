#include<QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "multithreadtcpserver.h"
#include "besconfigeditor.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     * Этот блок кода предназначен для обеспечения целостности конфигурационных файлов.
    * При первом запуске программы создаются все необходимые файлы. Администратору программы
    * остаётся лишь заполнить.
    * Создаём директорию для конфигурационных файлов
    */
    BesConfigEditor::createConfigDirectory();
    BesConfigEditor::createEmptyDatabaseConnectionConfig(DATABASE_CONFIG_FILE_NAME);
    BesConfigEditor::createEmptyServerConfig(SERVER_CONFIG_FILE_NAME);
    BesConfigEditor::createEmptyEmailSenderConfig(EMAIL_SENDER_CONFIG_FILE_NAME);

    databaseConnectionConfigEditor = new BesConfigEditor(DATABASE_CONFIG_FILE_NAME);
    serverConfigEditor = new BesConfigEditor(SERVER_CONFIG_FILE_NAME);
    emailSenderConfigEditor = new BesConfigEditor(EMAIL_SENDER_CONFIG_FILE_NAME);

    configureServer();
    configureViews();

    // Отображаем в UI параметры конфигурации, записанные в файле
    showConfigParameters();

    connect(this, SIGNAL(logMessage(QString)), SLOT(logToJournal(QString)));


}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
    delete databaseConnectionConfigEditor;
    delete serverConfigEditor;
    delete emailSenderConfigEditor;
}

void MainWindow::logToJournal(QString message)
{
    ui->logJournal->append(QTime::currentTime().toString("hh:mm:ss")+":"+message);
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

void MainWindow::configureServer()
{
    // Создаём многопоточный сервер
    server = new MultithreadTcpServer(QHostAddress::Any,
                                      serverConfigEditor,
                                      databaseConnectionConfigEditor,
                                      emailSenderConfigEditor);
    // Как только сервер запустился, это будет отображено в UI
    connect(server, SIGNAL(started()), SLOT(showServerStateAsActive()));
    connect(server, SIGNAL(stopped()), SLOT(showServerStateAsPassive()));
    // Прокидываем сообщение о регистрации сообщения в журнале
    connect(server, SIGNAL(logMessage(QString)),SIGNAL(logMessage(QString)));

    connect(server, SIGNAL(workingTimeUpdated(QString)),
            SLOT(updateServerWorkingTimeCounter(QString)));
}

void MainWindow::showConfigParameters()
{
    ui->databaseAddressEdit->setText(databaseConnectionConfigEditor->getString("address"));
    ui->databasePortEdit->setText(databaseConnectionConfigEditor->getString("port"));
    ui->databaseUserNameEdit->setText(databaseConnectionConfigEditor->getString("userName"));
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
        databaseConnectionConfigEditor->setValue("address", databaseAddress);
        databaseConnectionConfigEditor->setValue("port", databasePort);
        databaseConnectionConfigEditor->setValue("userName", userName);
        if(!password.isEmpty())
        {
            databaseConnectionConfigEditor->setValue("password", password);
        }
        // Сохраняем данные в файл конфигурации
        databaseConnectionConfigEditor->updateConfigFile();
        emit configParametersChanged();
    }
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


