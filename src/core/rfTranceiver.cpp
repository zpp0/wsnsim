/***********************************************
 *
 * File: rfTranceiver.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <iostream>

#include "env.h"
#include "message.h"
#include "nodeEventRadioRX.h"
#include "nodeEventRadioTX.h"
#include "rfTranceiver.h"

RfTranceiver::RfTranceiver(quint16 nodeID)
{
    m_ieeeAddr = 0;
    m_shortAddr = 0;
    m_DSN = 0;
    m_BSN = 0;
    m_PanC = false;
    m_PAN_ID = 0;

    m_nodeID = nodeID;
    
    m_channelNumber = 11;

    m_autoAck = false;

    busy = false;
}

quint64 RfTranceiver::wpanGetLongAddr()
{
    if (m_ieeeAddr == 0) {
        qsrand(m_nodeID);
        // 64 битный адрес узла
        m_ieeeAddr = ((quint64)qrand() << 32) + qrand();
    }
    return m_ieeeAddr;
}

// установка приемо-передатчика в режим координатора
void RfTranceiver::wpanSetPanC(bool PanC)
{
    if (!busy)
        m_PanC = PanC;
}

void RfTranceiver::wpanSetAutoAck(bool autoAck)
{
    if (!busy)
        m_autoAck = autoAck;
}

bool RfTranceiver::wpanGetAutoAck()
{
    return m_autoAck;
}

// установка короткого адреса устройства
void RfTranceiver::wpanSetShortAddr(quint16 shortAddr)
{
    if (!busy)
        m_shortAddr = shortAddr;
}

// установка идентификатора сети
void RfTranceiver::wpanSetPANID(quint16 PAN_ID)
{
    if (!busy)
        m_PAN_ID = PAN_ID;
}

// установка канала передачи данных
void RfTranceiver::wpanSetChannel(quint8 channel)
{
    if (!busy)
        m_channelNumber = channel;
}

// ожидание отправки сообщения
void RfTranceiver::wpanWaitForSend()
{
    
}

// отправка маячкового пакета
void RfTranceiver::wpanSendBeacon()
{
    if (!busy) {
        QVector<quint8> beaconBuffer;
        beaconBuffer += 0x40; //FCF[0:7]
        beaconBuffer += 0x80; // FCF[8:15]
        beaconBuffer += m_BSN++; // seq
        beaconBuffer += m_PAN_ID << 8; // Source PAN
        beaconBuffer += m_PAN_ID;
        beaconBuffer += m_shortAddr << 8; // Source PAN
        beaconBuffer += m_shortAddr;
        beaconBuffer += 0xFF; //Superframe Spec
        beaconBuffer += 3<<6;
        beaconBuffer += 0; // GTS spec
        beaconBuffer += 0; // GTS direction
        beaconBuffer += 0; // Pending addrs

        message* mes = new message(beaconBuffer);
        phyMesSend(mes);
    }
}

// отправка запроса на присоединение к сети
quint8 RfTranceiver::wpanAssocReq(quint16 coordAddr)
{
    if (!busy) {
        QVector<quint8> txBuffer;
        txBuffer += 0x63; //MAC command, Ack
        txBuffer += 0xC8; //DST: short+pan, SRC: long
        txBuffer += m_DSN;
        txBuffer += m_PAN_ID << 8;
        txBuffer += m_PAN_ID;
        txBuffer += coordAddr << 8;
        txBuffer += coordAddr;
        txBuffer += m_PAN_ID << 8;
        txBuffer += m_PAN_ID;
        txBuffer += m_ieeeAddr << 56;
        txBuffer += m_ieeeAddr << 48;
        txBuffer += m_ieeeAddr << 40;
        txBuffer += m_ieeeAddr << 32;
        txBuffer += m_ieeeAddr << 24;
        txBuffer += m_ieeeAddr << 16;
        txBuffer += m_ieeeAddr << 8;
        txBuffer += m_ieeeAddr;
        txBuffer += 0x01;
        txBuffer += 0x88;
        
        message* mes = new message(txBuffer);
        phyMesSend(mes);
    }
}

// отправка извещения о присоединении к сети
// nodeAddr - расширенный адрес присоединяемого узла
// shortAddr - его новый короткий адрес
quint8 RfTranceiver::wpanAssociate(quint64 nodeAddr, quint16 shortAddr, quint8 errorCode)
{
    if (!busy) {
        QVector<quint8> txBuffer;
        txBuffer += 0x63;
        txBuffer += 0xCC;
        txBuffer += m_DSN;
        txBuffer += m_PAN_ID << 8;
        txBuffer += m_PAN_ID;
        txBuffer += nodeAddr << 56;
        txBuffer += nodeAddr << 48;
        txBuffer += nodeAddr << 40;
        txBuffer += nodeAddr << 32;
        txBuffer += nodeAddr << 24;
        txBuffer += nodeAddr << 16;
        txBuffer += nodeAddr << 8;
        txBuffer += nodeAddr;
        txBuffer += m_PAN_ID << 8;
        txBuffer += m_PAN_ID;
        txBuffer += m_ieeeAddr << 56;
        txBuffer += m_ieeeAddr << 48;
        txBuffer += m_ieeeAddr << 40;
        txBuffer += m_ieeeAddr << 32;
        txBuffer += m_ieeeAddr << 24;
        txBuffer += m_ieeeAddr << 16;
        txBuffer += m_ieeeAddr << 8;
        txBuffer += m_ieeeAddr;
        txBuffer += 0x02;
        txBuffer += shortAddr << 8;
        txBuffer += shortAddr;
        txBuffer += errorCode;
        
        message* mes = new message(txBuffer);
        phyMesSend(mes);


        return m_DSN++;
    }
}
    
// Передача сообщение (данные)
// data - указатель на масив данных
// datalen - длина масива данных
// dest - получатель
// bAck - необходимость подтверждения о получении пакета
quint8 RfTranceiver::wpanSendData(QVector<quint8> &data, quint16 dest, bool Ack)
{
    if (!busy) {
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
}

void RfTranceiver::wpanSendAck(quint8 seqNum)
{
    if (!busy) {
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
}

QVector<quint8> RfTranceiver::wpanGetDataFromMessage(const QVector<quint8> &message)
{
    QVector<quint8> data;

    for (int i = 7; i < message.size(); i++) {
        // std::cerr << message[i];
        data += message[i];
    }

	// дальше идут собственно данные, возвращаем
	return data;
}

quint16 RfTranceiver::wpanGetShortDstAddressFromMessage(const QVector<quint8>& message)
{
	return ((quint16)message[3] << 8) + message[4];
    // std::cerr << message[4];
	// return (quint16)message[3];
}

quint16 RfTranceiver::wpanGetShortSrcAddressFromMessage(const QVector<quint8>& message)
{
    // std::cerr << message.size() << std::endl  << " \"";
    // for (int i = 0; i < message.size(); i++)
        // std::cerr << message[i];
	return ((quint16)message[5] << 8) + message[6];
    // std::cerr << "\" " << std::endl;

	// return (quint16)message[2];
}

void RfTranceiver::phyMesSend(message* mes)
{
    env::getChannel(m_channelNumber)->send(env::getNode(m_nodeID), mes);

    qDebug() << "create tx event";
    
    nodeEventRadioTX* eventTX = new nodeEventRadioTX(env::getMainTime(),
                                                     env::getNode(m_nodeID), 
                                                     mes,
                                                     env::getNode(m_nodeID)->getNodeType()->transmitterPower());
    
    eventTX->action();
    // env::insertEventQueue(eventTX);

    delete eventTX;

    env::setMainTime(env::getMainTime() + mes->getTransmitTime());

    // FIXME: убрать этот костыль
    env::moveNodes();
    env::nodesHearTest();
}

bool RfTranceiver::phyMesListen(message* mes)
{
    return env::getChannel(m_channelNumber)->listen(env::getNode(m_nodeID), mes);
}
