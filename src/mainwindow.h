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

private:
    Ui::MainWindow *ui;

    /// Многопоточный сервер, принимающий входящие пользовательские подключения
    /// и обрабатывающий пользовательские команды
    MultithreadTcpServer *server;
};

#endif // MAINWINDOW_H
