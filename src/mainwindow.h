#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "multithreadtcpserver.h"

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
private:
    /// Связывает элементы графического интерфейса окна с соответствующим слотами
    void configureViews();
    /// Сылается на объект представления окна серверного приложения
    Ui::MainWindow *ui;

    /// Многопоточный сервер, принимающий входящие пользовательские подключения
    /// и обрабатывающий пользовательские команды
    MultithreadTcpServer *server;
};

#endif // MAINWINDOW_H
