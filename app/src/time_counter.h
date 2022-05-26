#ifndef TIME_COUNTER_H
#define TIME_COUNTER_H
#include<QString>

#define SECONDS_TO_DAYS_FACTOR 86400
#define SECONDS_TO_HOURS_FACTOR 3600
#define SECONDS_TO_MINUTES_FACTOR 60
/// Используется для подсчёта времени
struct TimeCounter
{
private:
    unsigned long long seconds;
public:
    /// Ицициализирует счётчик времени 0
    TimeCounter();
    /// Возвращает время в виде строки в формате
    /// d д. hh ч. mm м. ss с.
    QString toString();
    /// Добавляет к счётчику секунду
    void addSecond();
};

#endif // TIME_COUNTER_H
