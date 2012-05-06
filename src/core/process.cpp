/***********************************************
 *
 * File: process.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <iostream>

#include "process.h"
#include "luaHandler.h"

#include "dataParams.h"

process::process(processParams* params)
{
    m_processID = params->processID;
    // получаем имя процесса
    m_name = params->name;
    // получаем путь к скрипту lua
    m_luaScript = params->luaScript;
    // получаем единицы измерений процесса
    m_units = params->units;

    // загружаем скрипт
    qDebug() << "process" << m_name << "load" << m_luaScript;
    
    luaHandler::loadScript(env::getLuaProc(), m_luaScript);
}

double process::measure(double coord[3], quint64 time)
{
    qDebug() << "measuring" << m_name;
    
    char *w = m_name.toLocal8Bit().data();
    // вытаскиваем функцию процеесса lua 
    lua_getglobal(env::getLuaProc(), w);

    // передаем в нее координаты и время
    lua_pushnumber(env::getLuaProc(), coord[0]);
    lua_pushnumber(env::getLuaProc(), coord[1]);
    lua_pushnumber(env::getLuaProc(), coord[2]);
    lua_pushnumber(env::getLuaProc(), time);

    //вызов функции процесса
    if (lua_pcall(env::getLuaProc(), 4, 1, 0) != 0)
        std::cerr << "Warning: process " << m_name.toStdString() << " measure error "
                  << " at coordinates " << coord[0] << coord[1] << coord[2]
                  << " time " << env::getMainTime() << std::endl;

    double rezult = lua_tonumber(env::getLuaProc(), -1);
    lua_pop(env::getLuaProc(), 1);
    
    return rezult;
}
