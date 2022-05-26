#ifndef SERVER_STATISTICS_COUNTER_H
#define SERVER_STATISTICS_COUNTER_H
// Автор: Сырцев Вадим Игоревич
#include <QThread>

///
/// Объекты данного класса будут хранить значение счётчиков некоторых параметров
/// сервера: количество активных подключений, количество обработанных запросов
///
class ServerStatisticsCounter : public QThread
{
    Q_OBJECT
public:
    ServerStatisticsCounter(QObject *parent = nullptr);
    /// Возвращает общее число установленных за текущий сеанс клиентских соединений
    /// с сервером
    unsigned long long getTotalEstablishedConnectionsCounter();
public slots:
    /// Увеличивает счётчик активных соединений на 1
    void increaseActiveConnectionsCounter();
    /// Уменьшает счётчик активных соединений на 1
    void decreaseActiveConnectionsCounter();
    /// Увеличивает счётчик общего числа установленных за текущий сеанс
    /// клиентских соединений с сервером
    void increaseTotalEstablishedConnectionsCounter();

signals:
    /// Высылается при изменении значения счётчика активных соединений
    void activeConnectionsCounterChanged(unsigned long long activeConnectionsCounter);

protected:
    void run();

private:
    /// Хранит количество активных в данных момент соединений
    unsigned long long activeConnectionsCounter;
    /// Хранит общее количество установленных соединений с сервером в течение текущей сессии
    unsigned long long totalEstablishedConnectionsCounter;
};

#endif // SERVER_STATISTICS_COUNTER_H
