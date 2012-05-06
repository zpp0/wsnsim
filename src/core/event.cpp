/***********************************************
 *
 * File: event.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include "event.h"

quint64 event::Count = 0;

// Конструктор принимает момент времени, в который должно произойти событие.
event::event(quint64 startTime)
{
    this->startTime = startTime;
    Count++;
}
