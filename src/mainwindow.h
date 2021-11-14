#ifndef MAINWINDOW_H
#define MAINWINDOW_H
// Автор: Сырцев Вадим Игоревич
#include <QMainWindow>
#include "multithreadtcpserver.h"
#include "configfileeditor.h"

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

signals:
    /// Сигнал, высылаемый после изменения пользователем параметров конфигурации
    /// в секции настроек
    void configParametersChanged();
private slots:
    /// Устанавливает значение счётчика активных клиентских подключений
    void setActiveConnectionsCounter(unsigned long long counter);
    /// Отображает в UI, что сервер работает
    void showServerStateAsActive();
    /// Отображает в UI, что сервер отключён
    void showServerStateAsPassive();
    /// Сохраняет установленные в UI параметры конфигурации в файл
    void saveConfigParameters();
    /// Отображает в UI текущие параметры конфигураци
    void showConfigParameters();

    /// Переключает состояние кнопок запуска и отсановки сервера.
    /// Меняет состояние кнопок местами
    void toggleStartStopBtns();
private:
    /// Связывает элементы графического интерфейса окна с соответствующим слотами
    void configureViews();
    /// Настраивает сервер и связывает его сигналы со слотами
    void configureServer(ConfigFileEditor *configParameters);

    /// Сылается на объект представления окна серверного приложения
    Ui::MainWindow *ui;

    /// Многопоточный сервер, принимающий входящие пользовательские подключения
    /// и обрабатывающий пользовательские команды
    MultithreadTcpServer *server;
    /// Обрабатывает параметры из файла конфигурации
    ConfigFileEditor configParameters;
};

#endif // MAINWINDOW_H
