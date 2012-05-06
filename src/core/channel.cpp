/***********************************************
 *
 * File: channel.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include "env.h"
#include "channel.h"
#include "node.h"
#include "message.h"
// #include "nodeEventRadioRXBegin.h"
#include "nodeEventRadioRX.h"

#include <iostream>

// длинна волны
double const LAMBDA = 0.122;

channel::channel(int frequency)
{
    m_frequency = frequency;
}

bool channel::listen(node* node, message* mes)
{
    int index = m_h_messagesListen[node].indexOf(mes);

    if (index == -1)
        return false;

    m_h_messagesListen[node].remove(index);

    bool islisten = isClearChannel(node);

    m_h_messagesListen.remove(node);

    return islisten;
}

void channel::send(node* node, message* mes)
{
    for (int i = 0; i < m_h_nodesListen[node].size(); i++) {

        m_h_messagesListen[m_h_nodesListen[node][i]] += mes;

        double rssi_value = rssi(node, m_h_nodesListen[node][i]);
        //добавим событие - принять сообщение, спустя время передачи
        // nodeEventRadioRXBegin* radioEvent;
        // radioEvent = new nodeEventRadioRXBegin(env::getMainTime(),
        //                                        m_h_nodesListen[node][i],
        //                                        mes,
        //                                        rssi_value);
        mes->RXCount++;
        nodeEventRadioRX* radioEvent;
        radioEvent = new nodeEventRadioRX(env::getMainTime() + mes->getTransmitTime(),
                                          m_h_nodesListen[node][i],
                                          mes,
                                          rssi_value);
        env::insertEventQueue(radioEvent);
    }
}

bool channel::isClearChannel(node* node)
{
    return m_h_messagesListen[node].isEmpty();
}

double channel::rssi(node* sender, node* listener)
{
    double dist = env::distance(sender, listener);

    double Gtx = 1.0;
    double Grx = 1.0;

    if (sender->getNodeType()->isDirectionalAntenna() == true) {
        double nu, fi;
        env::direction(sender, listener, dist, &nu, &fi);
        
        // вычисляем G на передающей антенне
        Gtx = sender->getG(nu, fi);

        // std::cerr <<  " id "  << sender->getMAC() <<  " Tx "  <<  " nu "  << nu <<  " fi "  << fi << std::endl;
    }
    
    if (listener->getNodeType()->isDirectionalAntenna() == true) {
        double nu, fi;
        env::direction(listener, sender, dist, &nu, &fi);

        // вычисляем G на приемной антенне
        Grx = listener->getG(nu, fi);

        // std::cerr <<  " id "  << listener->getMAC() <<  " Rx "  <<  " nu "  << nu <<  " fi "  << fi << std::endl;
    }

    // // std::cerr <<  " Gtx "  << Gtx <<  " Grx "  << Grx;

    // общее выражение Prx = Ptx*GTx*Grx(lambda/(4*pi*d))^2
    double rssi = sender->getNodeType()->transmitterPower() + 10 * log10(Grx * Gtx * pow(LAMBDA/(4*M_PI*dist), 2));

    // std::cerr <<  " sender "  << env::getNodeID(sender)
    // <<  " listener "  << env::getNodeID(listener)
              // <<  " rssi "  << rssi
              // <<  " distance "  << dist << std::endl;

    return rssi;
}

bool channel::hear(double rssi, node* listener)
{
    if (rssi >= listener->getNodeType()->receiverSensivity())
        return true;
    else
        return false;
}

//проверяем, слышат ли узлы друг друга
void channel::nodesHearTest(QVector<node*>& nodes)
{
    //составляем списки слышимых узлов для каждого из узлов
    for (int i = 0; i < nodes.size(); i++) {
        //для данного узла проверим видит ли каждого из остальных
        for (int j = 0; j < nodes.size(); j++) {

            //с самим собой не сравниваем
            if (i == j)
                continue;
            
            // FIXME: не наоборот надо?
            double Rssi = rssi(nodes[i], nodes[j]);
            
            // проверяем слышат ли друг друга узлы с адресами mac1 и mac2 на расстоянии d
            if ((hear(Rssi, nodes[j]) == true)
                && m_h_nodesListen[nodes[i]].indexOf(nodes[j]) == -1)
                changeLink(true, nodes[i], nodes[j], Rssi);

            // если не слышат, но раньше слышали
            else if ((hear(Rssi, nodes[j]) == false)
                     && m_h_nodesListen[nodes[i]].indexOf(nodes[j]) != -1)
                changeLink(false, nodes[i], nodes[j], Rssi);
        }
    }
}

void channel::changeLink(bool add, node* node1, node* node2, double rssi)
{
    if (add == true)
        // увеличить счетчик слышимых узлов данным
        m_h_nodesListen[node1].append(node2);

    else {
        int index = m_h_nodesListen[node1].indexOf(node2);
        m_h_nodesListen[node1].remove(index);
    }
    
    LogEvent event(EventType_CHANGELINK);
    event.m_time = env::getMainTime();
    event.m_nodeId = node1->getMAC();
    // event.m_pchangeLinkEventParam->id1 = node1;
    event.m_pchangeLinkEventParam->id2 = node2->getMAC();
    event.m_pchangeLinkEventParam->RSSI = rssi;
    
    log::writeLog(event);
}
