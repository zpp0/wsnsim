/***********************************************
 *
 * File: channel.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef CHANNEL_H
#define CHANNEL_H

#include "message.h"
#include "node.h"

class channel
{
public:
    channel(int frequency);

    bool listen(node* node, message* mes);
    void send(node* node, message* mes);
    bool isClearChannel(node* node);

    double rssi(node* sender, node* listener);

    void nodesHearTest(QVector<node*>& nodes);
    
private:
    int m_frequency;

    void changeLink(bool add, node* node1, node* node2, double rssi);
    
    bool hear(double rssi, node* listener);

    QHash<node*, QVector<node*> > m_h_nodesListen;

    QHash<node*, QVector<message*> > m_h_messagesListen;

};

#endif // CHANNEL_H
