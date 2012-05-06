/***********************************************
 *
 * File: process.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

#include "dataParams.h"

class process
{
public:
    process(processParams* params);
    double measure(double* coord, quint64 time);
    
private:

    quint16 m_processID;
    QString m_name;
    QString m_units;
    QString m_luaScript;
};

#endif // PROCESS_H
