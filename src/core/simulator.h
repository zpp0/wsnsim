/***********************************************
 *
 * File: simulator.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QString>

#include "log.h"

class simulator
{

public:
    simulator(QString projectFileName, log::LogFormat format);
    ~simulator();
    
    void eval();

    QString m_currentProjectPath;

private:
    void loadProject(QString file);
    static const quint16 m_version = 1;
};

#endif // SIMULATOR_H
