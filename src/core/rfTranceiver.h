/***********************************************
 *
 * File: rfTranceiver.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef RFTRANCEIVER_H
#define RFTRANCEIVER_H

#include <QVector>

#include "message.h"

class node;

class RfTranceiver
{
public:
    RfTranceiver(quint16 nodeID);

    // получение длинного адреса узла
    quint64 wpanGetLongAddr();

    // установка короткого адреса устройства
    void wpanSetShortAddr(quint16 shortAddr);

    // установка приемо-передатчика в режим координатора
    void wpanSetPanC(bool PanC);

    // установка приемо-передатчика в режим автоматического ответа
    void wpanSetAutoAck(bool autoAck);
    bool wpanGetAutoAck();

    // установка идентификатора сети
    void wpanSetPANID(quint16 PAN_ID);

    // установка канала передачи данных
    void wpanSetChannel(quint8 channel);

    // ожидание отправки сообщения
    void wpanWaitForSend();
    // отправка маячкового пакета
    void wpanSendBeacon();

    // отправка запроса на присоединение к сети
    quint8 wpanAssocReq(quint16 coordAddr);
    // отправка извещения о присоединении к сети
    // nodeAddr - расширенный адрес присоединяемого узла
    // shortAddr - его новый короткий адрес
    // errorCode - код ошибки, если присоединить узел невозможно

    // Передача извещения о присоединении к сети
    quint8 wpanAssociate(quint64 nodeAddr, quint16 shortAddr, quint8 errorCode);
    
    // Передача сообщение (данные)
    // data - указатель на масив данных
    // datalen - длина масива данных
    // dest - получатель
    // bAck - необходимость подтверждения о получении пакета
    quint8 wpanSendData(QVector<quint8> &data, quint16 dest, bool Ack);

    void wpanSendAck(quint8 dest);

    quint16 wpanGetShortDstAddressFromMessage(const QVector<quint8>& message);

    QVector<quint8> wpanGetDataFromMessage(const QVector<quint8> &message);
    quint16 wpanGetShortSrcAddressFromMessage(const QVector<quint8>& message);

    // quint16 wpanGetDstAddressFromMessage(const QVector<quint8>& message);

    // приемо-передатчик занят
    bool busy;

    // реальная физическая отправка сообщения
    void phyMesSend(message* mes);
    bool phyMesListen(message* mes);

private:
    node* m_node;

    quint64 m_ieeeAddr;
    quint16 m_shortAddr;
    // Data Sequence Number
    quint8 m_DSN;
    // Beacon Sequence Number
    quint8 m_BSN;
    // PAN Coordinator flag
    bool m_PanC;
    quint16 m_PAN_ID;
    
    quint8 m_channelNumber;

    bool m_autoAck;
};

#endif // RFTRANCEIVER_H
