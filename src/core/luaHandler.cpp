/***********************************************
 *
 * File: luaHandler.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <iostream>

#include "luaHandler.h"
#include "nodeEventRadioRXProcess.h"

node* luaHandler::currentNode = 0;
QString luaHandler::currentProjectPath = "";

void luaHandler::loadScript(lua_State *L, QString way)
{
    // Загружаем скрипт
    if (luaL_dofile (L, way.toUtf8().constData())) {
        std::cerr << "Warning: error load " << w << std::endl
                  << lua_tostring(L, -1) << std::endl;
        lua_pop(L, -1);
    }
    else
        qDebug() << "succsessfully load" << w;
}

lua_State* luaHandler::openLuaInterface()
{
    // открываем соединение
    lua_State *L = lua_open();

    // // подгружаем библиотеки
    // ага, точно
    // lua_pushcfunction(L, luaopen_math);
    // lua_pushstring(L, LUA_MATHLIBNAME);
    // lua_call(L, 1, 0);
    
    // lua_pushcfunction(L, luaopen_table);
    // lua_pushstring(L, LUA_TABLIBNAME);
    // lua_call(L, 1, 0);

    // lua_pushcfunction(L, luaopen_package);
    // lua_pushstring(L, LUA_LOADLIBNAME);
    // lua_call(L, 1, 0);
    
    // lua_pushcfunction(L, luaopen_io);
    // lua_pushstring(L, LUA_IOLIBNAME);
    // lua_call(L, 1, 0);

    // lua_pushcfunction(L, luaopen_os);
    // lua_pushstring(L, LUA_OSLIBNAME);
    // lua_call(L, 1, 0);

    // lua_pushcfunction(L, luaopen_string);
    // lua_pushstring(L, LUA_STRLIBNAME);
    // lua_call(L, 1, 0);

    // lua_pushcfunction(L, luaopen_debug);
    // lua_pushstring(L, LUA_DBLIBNAME);
    // lua_call(L, 1, 0);

    // lua_pushcfunction(L, luaopen_base);
    // lua_pushstring(L, LUA_LOADLIBNAME);
    // lua_call(L, 1, 0);

    luaL_openlibs(L);

    // функции для вызова из lua
    // lua_register(L, "log", luaHandler::log);

    lua_register(L, "timerInterruptInsert", luaHandler::timerInterraptInsert);

    // API для радио
    // отправка данных без использования стандарта
    lua_register(L, "rfMesSend", luaHandler::phyMesSend);
    // CCA
    lua_register(L, "CCA", luaHandler::CCA);
    // оповещение о получении полезного сообщения
    lua_register(L, "processing", luaHandler::processing);

    // // API для радио по стандарту
    // lua_register(L, "wpanSetPanC", luaHandler::wpanSetPanC);
    // lua_register(L, "wpanGetLongAddr", luaHandler::wpanGetLongAddr);
    lua_register(L, "wpanSetShortAddr", luaHandler::wpanSetShortAddr);
    // lua_register(L, "wpanSetPANID", luaHandler::wpanSetPANID);
    // lua_register(L, "wpanSendBeacon", luaHandler::wpanSendBeacon);
    // lua_register(L, "wpanAssocReq", luaHandler::wpanAssocReq);
    // lua_register(L, "wpanAssociate", luaHandler::wpanAssociate);
    lua_register(L, "wpanSendData", luaHandler::wpanSendData);
    lua_register(L, "wpanSendAck", luaHandler::wpanSendAck);

    // lua_register(L, "wpanGetDataFromMessage", luaHandler::wpanGetDataFromMessage);
    // lua_register(L, "wpanGetShortSrcAddressFromMessage", luaHandler::wpanGetShortSrcAddressFromMessage);
    lua_register(L, "wpanGetShortDstAddressFromMessage", luaHandler::wpanGetShortDstAddressFromMessage);
    
    // получение и установка данных в памяти узла
    lua_register(L, "dataSet", luaHandler::dataSet);
    lua_register(L, "dataGet", luaHandler::dataGet);

    // lua_register(L, "getMessageTransmitTime", luaHandler::getMessageTransmitTime);
    // lua_register(L, "getGlobalTime", luaHandler::getGlobalTime);

    // измерение процесса
    lua_register(L, "measure", luaHandler::measure);
    // получение времени на узле
    lua_register(L, "getLocalTime", luaHandler::localtime);
    // lua_register(L, "G", luaHandler::processing);

    QString luaModules = "package.path = package.path .. \";"
        + currentProjectPath + "/?.lua\"";
    
    luaL_dostring(L, luaModules.toUtf8().constData());

    // TODO: test this
    luaL_dostring(L, "jit.opt.start(3)");
    
    return L;
}

void luaHandler::closeLuaInterface(lua_State *L)
{
    // lua_State *L = currentL;
    lua_close(L);
}

// int luaHandler::log(lua_State *luaVM)
// {
//     // пишем в лог из lua
//     // const char* logText = lua_tostring(luaVM, 1);
//     // log::writeLog(currentNode, logText);
//     return 0;
// }

int luaHandler::CCA(lua_State *luaVM)
{
    // отправляем в lua результат проверки CCA
    
    int result = currentNode->CCA();
    lua_pushinteger(luaVM, result);
    return 1;
}

/*
принимает 3 параметра:
time - время, которое нужно подождать до преревания
type - тип прерывания
mesName - имя сообщения
*/
int luaHandler::timerInterraptInsert(lua_State *luaVM)
{
    // получаем из lua время, через которое сработает прерывание по таймеру
    quint64 time =  lua_tonumber(luaVM, 1);
    lua_pop(luaVM, 1);
    // получаем тип события (событие или процесс)
    const char* type =  lua_tostring(luaVM, 1);
    lua_pop(luaVM, 1);

    // получаем имя события
    // const char* mesName = lua_tostring(luaVM, 3);
    // вычисляем время симулятора, когда произойдет событие
    // quint64 startTime = ;

    // std::cout << "event_node_ptr " << currentNode << "\n";
    // добавляем событие в очередь
    currentNode->innerInteruptInsert(time);
    // currentNode->innerInteruptInsert(startTime, type, mesName);

    return 0;
}

int luaHandler::phyMesSend(lua_State *luaVM)
{
    // читаем тело сообщения
    QVector<quint8> body;
    QHash<int,int> tmp;
    if (lua_istable(luaVM,1)) {
        // int i;
        // char mes[255];
        lua_pushnil(luaVM);
        
        while (lua_next(luaVM, 1)) {
           tmp[lua_tonumber(luaVM, -1)] = lua_tonumber(luaVM, -1);
           //body.push_back(  lua_tonumber(luaVM, -1) ); // ] = lua_type(luaVM, -1);
           //print key value
           // удаляем значение value из стэка, что-бы на следующей итерации
           // ключ использовался для нахождения следующей пары значений из таблицы
           lua_pop(luaVM, 1);
        }
    }
    QHash<int,int>::iterator order;

    for (order = tmp.begin(); order != tmp.end(); ++order)
        body.push_back(order.value());

    message* mes = new message(body);
    // std::cerr << "message" << mes << std::endl;
    currentNode->phyMesSend(mes);
    return 0;
}

int luaHandler::dataSet(lua_State *luaVM)
{
    if (lua_istable(luaVM,2)) {
        char arrayKeyChar[256],a[256];
        const char* arrayKey;
        int arrayKeyInt,numberKey;
        QString strKey;
        
        if (lua_isnumber(luaVM, 1)) {
            arrayKeyInt = lua_tonumber(luaVM, 1);
            sprintf(arrayKeyChar, "%i", arrayKeyInt);
            arrayKey = arrayKeyChar;
        }
        else
            arrayKey = lua_tostring(luaVM, 1);
        
        lua_pushnil(luaVM);
        
        while (lua_next(luaVM, 2)) {
            
         /*   log::write(env::init(),"value---------------------");
            log::write(env::init(),lua_tostring(luaVM, -1) );
            log::write(env::init(),"key---------------------");
            log::write(env::init(),lua_tostring(luaVM, -2) );
            */
            
            if (lua_isnumber(luaVM, -2)) {
                numberKey = lua_tonumber(luaVM, -2);
                sprintf(a, "%i", numberKey);
                strKey = a;
            }
            else
                strKey = lua_tostring(luaVM, -2);

            currentNode->dataArStr[arrayKey][strKey] = lua_tostring(luaVM, -1);

            lua_pop(luaVM, 1);
        }
    }
    else
        currentNode->dataStr[ lua_tostring(luaVM, 1) ] = lua_tostring(luaVM, 2);

    return 0;
}

int luaHandler::dataGet(lua_State *luaVM)
{
    QString key = lua_tostring(luaVM, 1);

    QString data = currentNode->dataStr[key];
    QHash<QString,QString> dataAr = currentNode->dataArStr[key];
    
    // если данные стороквые
    if (data != "") {
        //просто кладём строку в очередь
        char* dataResult = data.toLocal8Bit().data();
        lua_pushstring(luaVM,dataResult);
        return 1;
    }
    // если данные являются массивом
    else if (dataAr.size() > 0) {
        lua_newtable(luaVM);
        QString key, value;
        QHash<QString,QString>::iterator i;

        for (i = dataAr.begin(); i != dataAr.end(); ++i) {
            key = i.key();
            lua_pushstring(luaVM,key.toLocal8Bit().data());
            value = i.value();
            lua_pushstring(luaVM,value.toLocal8Bit().data());
            lua_settable(luaVM, -3);
        }
    }
    else
        lua_pushstring(luaVM,"");

    return 1;
}

//функция принимает сообщение и возвращает время, которое понадобится
//на то, чтобы передать это сообщение
// принимает из lua
int luaHandler::getMessageTransmitTime(lua_State *luaVM)
{
    // читаем тело сообщения
    QVector<quint8> body;
    QHash<int,int> tmp;

    if (lua_istable(luaVM,1)) {
        // int i;
        // char mes[255];
        lua_pushnil(luaVM);
        while (lua_next(luaVM, 1)) {
           tmp[lua_tonumber(luaVM, -2)] = lua_tonumber(luaVM, -1);
           lua_pop(luaVM, 1);
        }
    }

    QHash<int,int>::iterator order;
    for (order = tmp.begin(); order != tmp.end(); ++order)
        body.push_back(order.value());
    
    // создаём сообщение и считаем время на его передачу
    message* mes = new message(body);
    int time = mes->getTransmitTime();
    lua_pushnumber(luaVM, time);
    delete mes;

    return 1;
}

// int luaHandler::getGlobalTime(lua_State *luaVM)
// {
//     lua_pushnumber(luaVM, env::getMainTime());
//     return 1;
// }

int luaHandler::measure(lua_State *luaVM)
{
    // получаем номер датчика
    QString sensorName = lua_tostring(luaVM, 1);

    
    double value = currentNode->measure(sensorName);

    // sensor* requirmentSensor = currentNode->sensors.value(sensorName);
    
    // if (requirmentSensor != NULL) {
    //     // проводим измерение
    //     double rez = requirmentSensor->measure();
        
        // отправляем полученный результат в lua
    lua_pushnumber(luaVM, value);
    // }

    // else {
    //     QList<QString> sensorsNames = currentNode->sensors.keys();
    //     std::cerr << "WARNING: There is no sensor named "
    //               << sensorName.toStdString()
    //               << " as required from node "
    //               << currentNode->getMAC()
    //               << " at time " << env::getMainTime() << std::endl;
    //     std::cerr << "Sensors list: [ ";
    //     for (int i = 0; i < sensorsNames.size(); i++)
    //         std::cerr << "  " << sensorsNames[i].toStdString() << " ";
    //     std::cerr << " ]" << std::endl;
    // }
    
    return 1;
}

int luaHandler::localtime(lua_State *luaVM)
{
    // проводим измерение
    quint64 rez = currentNode->localtime();
    
    // отправляем полученный результат в lua
    lua_pushnumber(luaVM, rez);
    
    return 1;
}

int luaHandler::processing(lua_State *luaVM)
{

    nodeEventRadioRXProcess* event =
        new nodeEventRadioRXProcess(env::getMainTime(),
                                    currentNode);

    event->action();

    delete event;
    // env::insertEventQueue(event);

    return 1;
}

// int luaHandler::wpanGetLongAddr(lua_State *luaVM)
// {
//     quint64 ieeeAddr = currentNode->rfTranceiver->wpanGetLongAddr();
    
//     lua_pushinteger(luaVM, ieeeAddr);
//     return 1;
// }

// int luaHandler::wpanSetPanC(lua_State *luaVM)
// {
//     bool PanC = lua_toboolean(luaVM, 1);
//     lua_pop(luaVM, 1);

//     currentNode->rfTranceiver->wpanSetPanC(PanC);
//     return 1;
// }

int luaHandler::wpanSetShortAddr(lua_State *luaVM)
{
    quint16 shortAddr = lua_tonumber(luaVM, 1);
    lua_pop(luaVM, 1);

    currentNode->wpanSetShortAddr(shortAddr);
    return 1;
}

// int luaHandler::wpanSetPANID(lua_State *luaVM)
// {
//     quint16 PANID = lua_tonumber(luaVM, 1);
//     lua_pop(luaVM, 1);
//     currentNode->rfTranceiver->wpanSetPANID(PANID);
//     return 1;
// }

// int luaHandler::wpanSendBeacon(lua_State *luaVM)
// {
//     currentNode->rfTranceiver->wpanSendBeacon();
//     return 1;
// }

// int luaHandler::wpanAssocReq(lua_State *luaVM)
// {
//     quint16 coordAddr = lua_tonumber(luaVM, 1);
//     lua_pop(luaVM, 1);
//     quint8 ack = currentNode->rfTranceiver->wpanAssocReq(coordAddr);
//     lua_pushinteger(luaVM, ack);
//     return 1;
// }

// int luaHandler::wpanAssociate(lua_State *luaVM)
// {
//     quint64 nodeAddr = lua_tonumber(luaVM, 1);
//     lua_pop(luaVM, 1);

//     quint16 shortAddr = lua_tonumber(luaVM, 1);
//     lua_pop(luaVM, 1);
//     quint8 errorCode = lua_tonumber(luaVM, 1);
//     lua_pop(luaVM, 1);
    
//     quint8 ack = currentNode->rfTranceiver->wpanAssociate(nodeAddr, shortAddr, errorCode);

//     lua_pushinteger(luaVM, ack);
//     return 1;
// }

int luaHandler::wpanSendData(lua_State *luaVM)
{
    // читаем тело сообщения
    QVector<quint8> body;
    QHash<int,int> tmp;
    if (lua_istable(luaVM,1)) {
        // int i;
        // char mes[255];
        lua_pushnil(luaVM);
        
        while (lua_next(luaVM, 1)) {
           tmp[lua_tonumber(luaVM, -1)] = lua_tonumber(luaVM, -1);
           //body.push_back(  lua_tonumber(luaVM, -1) ); // ] = lua_type(luaVM, -1);
           //print key value
           // удаляем значение value из стэка, что-бы на следующей итерации
           // ключ использовался для нахождения следующей пары значений из таблицы
           lua_pop(luaVM, 1);
        }
    }
    QHash<int,int>::iterator order;

    for (order = tmp.begin(); order != tmp.end(); ++order)
        body.push_back(order.value());

    quint16 dest = lua_tonumber(luaVM, 2);
    bool bAck = lua_toboolean(luaVM, 3);
    quint8 ack = currentNode->wpanSendData(body, dest, bAck);
    
    lua_pushinteger(luaVM, ack);

    // lua_gc(luaVM, LUA_GCCOLLECT, 0);
    
    return 1;
}

int luaHandler::wpanSendAck(lua_State *luaVM)
{
    // читаем тело сообщения
    QVector<quint8> body;
    if (lua_istable(luaVM,1)) {
        lua_pushnil(luaVM);
        
        while (lua_next(luaVM, 1)) {
            body += lua_tonumber(luaVM, -1);
            lua_pop(luaVM, 1);
        }
    }

    quint8 ack = lua_tonumber(luaVM, 1);
    currentNode->wpanSendAck(ack);
    
    return 1;
}

// int luaHandler::wpanGetDataFromMessage(lua_State *luaVM)
// {
//     // читаем тело сообщения
//     QVector<quint8> body;
//     if (lua_istable(luaVM,1)) {
//         lua_pushnil(luaVM);
        
//         while (lua_next(luaVM, 1)) {
//             body += lua_tonumber(luaVM, -1);
//             // удаляем значение value из стэка, что-бы на следующей итерации
//             // ключ использовался для нахождения следующей пары значений из таблицы
//             lua_pop(luaVM, 1);
//         }
//     }

//     QVector<quint8> data = currentNode->rfTranceiver->wpanGetDataFromMessage(body);

//     //кладём в стек тело сообщения, как таблицу
//     lua_newtable(luaVM);

//     // передаем тело сообщения на lua
//     for (int i = 0; i < data.size(); i++) {
//         lua_pushnumber(luaVM, i + 1);
//         lua_pushnumber(luaVM, data[i]);
//         lua_settable(luaVM, -3);
//     }
    
//     return 1;
// }

// int luaHandler::wpanGetShortSrcAddressFromMessage(lua_State *luaVM)
// {
//     // читаем тело сообщения
//     QVector<quint8> body;
//     QHash<int,int> tmp;
//     if (lua_istable(luaVM,1)) {
//         // int i;
//         // char mes[255];
//         lua_pushnil(luaVM);
        
//         while (lua_next(luaVM, 1)) {
//             body += lua_tonumber(luaVM, -1);
//            //body.push_back(  lua_tonumber(luaVM, -1) ); // ] = lua_type(luaVM, -1);
//            //print key value
//            // удаляем значение value из стэка, что-бы на следующей итерации
//            // ключ использовался для нахождения следующей пары значений из таблицы
//            lua_pop(luaVM, 1);
//         }
//     }

//     // QHash<int,int>::iterator order;

//     // for (order = tmp.begin(); order != tmp.end(); ++order) {
//     //     // std::cerr << order.value() << " ";
//     //     body.push_back(order.value());
//     // }
//     // // std::cerr << std::endl;

//     quint16 srcAddr = currentNode->rfTranceiver->wpanGetShortSrcAddressFromMessage(body);

//     // //кладём в стек тело сообщения, как таблицу
//     // lua_newtable(luaVM);

//     // std::cerr << srcAddr;
//     // передаем адрес на lua
//     lua_pushnumber(luaVM, srcAddr);
    
//     return 1;
// }

int luaHandler::wpanGetShortDstAddressFromMessage(lua_State *luaVM)
{
    // читаем тело сообщения
    QVector<quint8> body;
    QHash<int,int> tmp;
    if (lua_istable(luaVM,1)) {
        // int i;
        // char mes[255];
        lua_pushnil(luaVM);
        
        while (lua_next(luaVM, 1)) {
           body += lua_tonumber(luaVM, -1);
           //body.push_back(  lua_tonumber(luaVM, -1) ); // ] = lua_type(luaVM, -1);
           //print key value
           // удаляем значение value из стэка, что-бы на следующей итерации
           // ключ использовался для нахождения следующей пары значений из таблицы
           lua_pop(luaVM, 1);
        }
    }

    // QHash<int,int>::iterator order;

    // for (order = tmp.begin(); order != tmp.end(); ++order) {
    //     // std::cerr << order.value() << " ";
    //     body.push_back(order.value());
    // }
    // // std::cerr << std::endl;
    
    quint16 dstAddr = currentNode->wpanGetShortDstAddressFromMessage(body);

    // std::cerr << dstAddr;

    // //кладём в стек тело сообщения, как таблицу
    // lua_newtable(luaVM);

    // передаем адрес на lua
    lua_pushnumber(luaVM, dstAddr);
    
    return 1;
}

// int luaHandler::wpanSetAutoAck(lua_State *luaVM)
// {
//     bool enableAutoAck = lua_toboolean(luaVM, 1);
//     lua_pop(luaVM, 1);

//     currentNode->rfTranceiver->wpanSetAutoAck(enableAutoAck);
    
//     return 1;
// }
