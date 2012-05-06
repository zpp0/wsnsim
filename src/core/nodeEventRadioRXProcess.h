/***********************************************
 *
 * File: nodeEventRadioRXProcess.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENTRADIORXPROCESS_H
#define NODEEVENTRADIORXPROCESS_H

#include "QVector"

#include "nodeEvent.h"
#include "luaHandler.h"

class nodeEventRadioRXProcess : public nodeEvent
{
public:
    nodeEventRadioRXProcess(quint64 startTime, node *eventNode);
    virtual void action();
    virtual bool check() { return true; }
};

#endif // NODEEVENTRADIORXPROCESS_H
