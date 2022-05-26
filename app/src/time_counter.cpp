#include "time_counter.h"
#include <QDebug>

TimeCounter::TimeCounter()
{
    seconds=0;
}

QString TimeCounter::toString()
{
    unsigned long long secondsCopy = seconds;
    int days = secondsCopy/SECONDS_TO_DAYS_FACTOR;
    secondsCopy %=SECONDS_TO_DAYS_FACTOR;

    int hours = secondsCopy/SECONDS_TO_HOURS_FACTOR;
    secondsCopy %=SECONDS_TO_HOURS_FACTOR;

    int minutes = secondsCopy/SECONDS_TO_MINUTES_FACTOR;
    secondsCopy %=SECONDS_TO_MINUTES_FACTOR;
    return QString("%1 д. %2 ч. %3 м. %4 с.")
            .arg(days, 3)
            .arg(hours, 2)
            .arg(minutes, 2)
            .arg(secondsCopy, 2);

}

void TimeCounter::addSecond()
{
    seconds++;
}
