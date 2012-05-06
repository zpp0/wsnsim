/***********************************************
 *
 * File: luaHandler.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef LUAHANDLER_H
#define LUAHANDLER_H

#include <QVector>
#include <QHash>
#include <QString>

#include "log.h"
#include "env.h"
#include "node.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

/*
  класс для работы со скриптами lua.
  содержит все методы, которые вызываются из lua скриптов
 */
class luaHandler
{
public:
    // метод загружает скрипт, путь к которому указан в аргументе way
    static void loadScript(lua_State *L, QString way);
    // текущий узел
    static node* currentNode;
    static QString currentProjectPath;
    // открыть соединение
    static lua_State* openLuaInterface();
    // закрыть соединение
    static void closeLuaInterface(lua_State *L);

private:
    // основые функции интерфейса Lua скриптов
    
    // записать сообщение в лог
    // записать состояние интерпретатора в лог
    static int log(lua_State *luaVM);
    // физическая отправка сообщения
    // метод принимает из lua адрес отправителя и тело сообщения,
    // отправляет ядру команду об отправке сообщения
    static int phyMesSend(lua_State *luaVM);
    // процедура ССА
    // отправляет результат проверки CCA из ядра в lua
    static int CCA(lua_State *luaVM);
    // запланировать прерывание по таймеру
    // принимает из lua параметры прерывания и запоминает их в ядре
    static int timerInterraptInsert(lua_State *luaVM);
    // записать информацию на узел
    // записывает данные из lua в currentEvent->eventNode->dataArStr[ arrayKey ][ strKey ]
    static int dataSet(lua_State *luaVM);
    // прочитать информацию с узла
    // отправляет данные из currentEvent->eventNode->dataStr на lua
    static int dataGet(lua_State *luaVM);

    // вспомогательные функции

    // узнать, сколько времени будет передавать сообщение
    // принимает из lua сообщение, рассчитывает время передачи, отправляет его обратно на lua
    static int getMessageTransmitTime(lua_State *luaVM);
    // получить глобальное время внешней среды
    // отправляет на lua глобальное время среды
    static int getGlobalTime(lua_State *luaVM);

    // получить измерения с датчика
    // принимает из lua номер датчика
    // возвращает в lua значение с датчика
    static int measure(lua_State *luaVM);

    // получить локальное значение часов на узле
    // входных аргументов из lua нет
    // отправляет на lua локальное время узла
    static int localtime(lua_State *luaVM);
    
    // узел обрабатывает сообщение
    static int processing(lua_State *luaVM);

    /* static int wpanSetAutoAck(lua_State *luaVM); */
    /* static int wpanSetPanC(lua_State *luaVM); */
    /* static int wpanGetLongAddr(lua_State *luaVM); */
    static int wpanSetShortAddr(lua_State *luaVM);
    /* static int wpanSetPANID(lua_State *luaVM); */
    /* static int wpanSendBeacon(lua_State *luaVM); */
    /* static int wpanAssocReq(lua_State *luaVM); */
    /* static int wpanAssociate(lua_State *luaVM); */
    static int wpanSendData(lua_State *luaVM);
    static int wpanSendAck(lua_State *luaVM);

    /* static int wpanGetDataFromMessage(lua_State *luaVM); */
    static int wpanGetShortDstAddressFromMessage(lua_State *luaVM);
    /* static int wpanGetShortSrcAddressFromMessage(lua_State *luaVM); */
    
};

#endif // LUAHANDLER_H
