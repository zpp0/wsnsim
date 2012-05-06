/***********************************************
 *
 * File: message.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QVector>

class message
{
public:
    // конструкторы
    // сообщение без обратного адреса
    message(QVector<quint8>& body);
    
    // метод возвращает тело сообщения
    QVector<quint8>& getBody();
    QVector<quint8>& getMessage();
    
    //сколько времени потребуется на передачу сообщения
    quint64 getTransmitTime();

    // костыль чтобы можно было удалить сообщение
    // счетчик оставшихся узлов, которые еще не обработали это сообщение
    quint16 RXCount;

private:

    //тело сообщения
    QVector<quint8> m_body;
    QVector<quint8> m_message;
    //длинна сообщения
    quint8 m_length;
    quint64 m_preambule;
    quint8 m_SFD;
};

#endif // MESSAGE_H
