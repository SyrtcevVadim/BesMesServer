#ifndef MAINWINDOW_H
#define MAINWINDOW_H
// Автор: Сырцев Вадим Игоревич
#include <QMainWindow>
#include "multithreadtcpserver.h"
#include "besconfigreader.h"


// Текст, отображаемый в UI, если сервер работает
#define ACTIVE_SERVER_STATE_TEXT QObject::tr("включён")
// Текст, отображаемый в UI, если сервер отключён
#define PASSIVE_SERVER_STATE_TEXT QObject::tr("выключен")



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /// Устанавливает значение счётчика активных клиентских подключений
    void setActiveConnectionsCounter(unsigned long long counter);
    /// Отображает в UI, что сервер работает
    void showServerStateAsActive();
    /// Отображает в UI, что сервер отключён
    void showServerStateAsPassive();

    /// Отображает в UI текущие параметры конфигураци
    void showConfigParameters();

    /// Переключает состояние кнопок запуска и отсановки сервера.
    /// Меняет состояние кнопок местами
    void toggleStartStopBtns();
    /// Обновляет в UI метку-счётчик, отвечающую за хранения времени работы
    /// приложения в данной сессии
    void updateServerWorkingTimeCounter(QString time);
private:
    /// Связывает элементы графического интерфейса окна с соответствующим слотами
    void configureViews();
    /// Настраивает многопоточный tls-сервер
    void configureServer();
    /// Сылается на объект представления окна серверного приложения
    Ui::MainWindow *ui;
    /// Многопоточный сервер, принимающий входящие пользовательские подключения
    /// и обрабатывающий пользовательские команды
    MultithreadTcpServer *server;
};

#endif // MAINWINDOW_H
