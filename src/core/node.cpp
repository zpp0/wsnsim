/***********************************************
 *
 * File: node.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <iostream>

#include "node.h"
#include "env.h"
#include "log.h"
#include "nodeEventRadioTX.h"
#include "nodeEventTimer.h"
#include "nodeEventLocalTime.h"
#include "api.h"

node::node(quint16 ID, nodeType* nt)
{
    //сразу присваиваем ID
    m_MAC_ID = ID;
    m_nt = nt;

    // записываем в данные узла его MAC адрес в текстовом виде
    // будет использоваться в lua
    dataStr["ID"] = QString::number(m_MAC_ID);

    qsrand(m_MAC_ID);
    // 64 битный адрес узла
    m_ieeeAddr = ((quint64)qrand() << 32) + qrand();

    m_shortAddr = 0;

    m_DSN = 0;

    // rfTranceiver = new RfTranceiver(m_MAC_ID);

    // устанавливаем на узел датчики
    QVector<sensorParams*> sensorParams = m_nt->sersorParamsVector();
    
    for (int i = 0; i < sensorParams.size(); i++) {
        // создаем датчик
        // sensor* s = new sensor(sensorParams[i], this);
        // устанавливаем датчик на узел
        sensors.insert(sensorParams[i]->name, sensorParams[i]->processID);
    }

    m_channelNumber = 11;

    isTimerEnabled = false;
        
    // создаем lua VM для программ этого типа узлов
    m_lua = luaHandler::openLuaInterface();

    // qDebug() << "node" << m_nt->name() << "load" << params->luaFile;
    luaHandler::loadScript(m_lua, m_nt->luaFile());
}

node::~node()
{
    luaHandler::closeLuaInterface(m_lua);
}

lua_State* node::getLua()
{
    return m_lua;
}

bool node::CCA()
{
    return env::getChannel(m_channelNumber)->isClearChannel(this);
}

double node::measure(QString sendorName)
{
    // FIXME: request undefined sensor
    return env::measure(this, sensors[sendorName]);
}

void node::innerInteruptInsert(quint64 startTime)
{
    qDebug() << "create inner event";

    if (isTimerEnabled == true) {
        std::cerr << "Warning: cann't setup more than one timer. Request will be ignored." << std::endl;
        return;
    }
    
    nodeEventTimer* event;
    if (m_nt->isRealClock() == true) {

        quint64 localTime = env::nodeLocaltime(this, env::getMainTime());
        quint64 localStartTime = localTime + startTime;
        quint64 globalStartTime = env::nodeGlobaltime(this, localStartTime);

        event = new nodeEventTimer(globalStartTime, this, "");
        
        qDebug() << "node real clock"
                 << "local time" << env::nodeLocaltime(this, env::getMainTime())
                 << "local start time" << env::nodeLocaltime(this, env::getMainTime()) + startTime
                 << "global start time" << globalStartTime;

    }
    else
        event = new nodeEventTimer(env::getMainTime() + startTime, this, "");

    // std::cerr << "event " << event << std::endl;
    env::insertEventQueue(event);

    isTimerEnabled = true;
}

void node::phyMesSend(message* mes)
{
    env::getChannel(m_channelNumber)->send(this, mes);

    qDebug() << "create tx event";
    
    nodeEventRadioTX* eventTX = new nodeEventRadioTX(env::getMainTime(),
                                                     this,
                                                     mes,
                                                     m_nt->transmitterPower());
    
    eventTX->action();
    // env::insertEventQueue(eventTX);

    delete eventTX;

    env::setMainTime(env::getMainTime() + mes->getTransmitTime());

    // FIXME: убрать этот костыль
    env::moveNodes();
    env::nodesHearTest();
}

bool node::phyMesListen(message* mes)
{
    return env::getChannel(m_channelNumber)->listen(this, mes);
}


double node::getG(double nu, double fi)
{
    // вытаскиваем функцию процеесса lua 
    lua_getglobal(getLua(), m_nt->functionName(NODE_DIRECTIONAL_ANTENNA));

    // передаем в нее координаты и время
    lua_pushnumber(getLua(), nu);
    lua_pushnumber(getLua(), fi);
    lua_pushnumber(getLua(), env::getMainTime());

    //вызов функции процесса
    if (lua_pcall(getLua(), 3, 1, 0) != 0)
        std::cerr << "Warning: node " << m_MAC_ID << " computing G error "
                  << " with nu " << nu << " fi " << fi
                  << " at time " << env::getMainTime() << std::endl;

    double rezult = lua_tonumber(getLua(), -1);
    lua_pop(getLua(), 1);
    
    return rezult;
}

quint64 node::localtime()
{
    quint64 time;
    
    // если узел использует реальную модель часов
    if (m_nt->isRealClock() == true)
        // получаем локальное время
        time = env::nodeLocaltime(this, env::getMainTime());
    // не использует ->
    else
        // на узле глобальное время
        time = env::getMainTime();

    // создаем событие
    nodeEventLocalTime* event = new nodeEventLocalTime(env::getMainTime(), this, time);

    event->action();
    // env::insertEventQueue(event);

    // возвращаем
    return time;
}

//функции для управления наиболее значимыми свойствами - get и set
nodeType* node::getNodeType()
{
    return m_nt;
}

quint16 node::getMAC()
{
    return m_MAC_ID;
}

quint16 node::wpanGetShortDstAddressFromMessage(const QVector<quint8>& message)
{
	return ((quint16)message[3] << 8) + message[4];
}

void node::wpanSendAck(quint8 seqNum)
{
    // Формируем пакет в соответсвии со стандартом
    QVector<quint8> txBuffer;
    // Сообщение с данными, внутрисетевое
    // txBuffer += 0x41;
    // txBuffer += 0x40;
    txBuffer += 0x02;
    txBuffer += 0x0;

    // номер посылки
    txBuffer += seqNum;
    
    message* mes = new message(txBuffer);
    phyMesSend(mes);
}

quint8 node::wpanSendData(QVector<quint8> &data, quint16 dest, bool Ack)
{

    // Формируем пакет в соответсвии со стандартом
    QVector<quint8> txBuffer;
    // Сообщение с данными, внутрисетевое
    // txBuffer += 0x41;
    txBuffer += 0x01;
    // Требуется подтверждение
    if (Ack)
        txBuffer[0] |= 0x20;

    txBuffer += 0x22;
    // if (m_PanC)
    //     // Сообщение от координатора, адрес координатора опущен
    //     txBuffer += 0x08;
    // else
    //     // Сообщение для координатора, адрес координатора опущен
    //     txBuffer += 0x80;

    // номер посылки
    txBuffer += m_DSN;

    // // Сообщение от координатора?
    // if (m_PanC) {
    //     // Да, Записываем адрес сети и локальный адрес получателя
    //     txBuffer += m_PAN_ID << 8;
    //     txBuffer += m_PAN_ID;
    //     txBuffer += dest << 8;
    //     txBuffer += dest;
    //     std::cerr << dest << std::endl;
    // }
    // else {
    //     // Нет, Записываем адрес сети и локальный адрес отправителя
    //     txBuffer += m_PAN_ID << 8;
    //     txBuffer += m_PAN_ID;
    //     txBuffer += m_shortAddr << 8;
    //     txBuffer += m_shortAddr;
    // }

        
    // txBuffer += m_PAN_ID << 8;
    // txBuffer += m_PAN_ID;
    txBuffer += dest << 8;
    txBuffer += dest;

    txBuffer += m_shortAddr << 8;
    txBuffer += m_shortAddr;
            
    txBuffer += data;
    
    message* mes = new message(txBuffer);
    phyMesSend(mes);

    return m_DSN++;
}

void node::wpanSetShortAddr(quint16 shortAddr)
{
    m_shortAddr = shortAddr;
}
