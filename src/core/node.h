/***********************************************
 *
 * File: node.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QVector>
#include <QHash>

#include "event.h"
#include "message.h"
#include "nodeType.h"
/* #include "sensor.h" */

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class node
{

public:
    // СЛУЖЕБНОЕ

    node(quint16 ID, nodeType* nt);
    ~node();

    // МЕТОДЫ ДЛЯ УПРАВЛЕНИЯ ДОСТУПОМ К ОСНОВНЫМ СВОЙСТВАМ
    nodeType* getNodeType();

    quint16 getMAC();

    lua_State* getLua();
    
    // проверка - свободен ли канал вокруг узла
    bool CCA();

    // ДАННЫЕ НА УЗЛАХ, КОТОРЫМИ УПРАВЛЯЮТ СКРИПТЫ ПОЛЬЗОВАТЕЛЕЙ
    QHash<QString,QString> dataStr;
    QHash<QString,QHash<QString,QString> > dataArStr;

    /* QHash<QString, sensor*> sensors; */

    double measure(QString sensorName);

    // запланироваь ну узле прерывание по таймеру
    void innerInteruptInsert(quint64 waitTime);

    quint64 localtime();
    
    double getG(double nu, double fi);

    bool isTimerEnabled;

    quint8 wpanSendData(QVector<quint8> &data, quint16 dest, bool Ack);
    void wpanSendAck(quint8 dest);
    quint16 wpanGetShortDstAddressFromMessage(const QVector<quint8>& message);
    void wpanSetShortAddr(quint16 shortAddr);

    void phyMesSend(message* mes);
    bool phyMesListen(message* mes);
    
private:

    // мак адрес узла
    quint16 m_MAC_ID;

    QHash<QString, quint16> sensors;

    quint16 m_channelNumber;
    
    quint8 m_DSN;

    quint64 m_ieeeAddr;
    quint16 m_shortAddr;

    // тип узла
    nodeType* m_nt;

    lua_State* m_lua;
};

#endif // NODE_H
