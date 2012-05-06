/***********************************************
 *
 * File: message.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <QVector>
#include <QString>
#include <math.h>
#include <cassert>
#include "message.h"
#include "env.h"
#include "log.h"

// байт / микроСекунду
double const V = 0.03125;

message::message(QVector<quint8> &body)
{
    assert(body.size() > 0);

    m_length = body.size();
    
    m_body = body;

    m_message += m_length;
    m_message += m_body;

    m_SFD = 0x7A;
    m_preambule = 0x0;

    RXCount = 0;
}

QVector<quint8>& message::getBody()
{
    return m_body;
}

QVector<quint8>& message::getMessage()
{
    return m_message;
}

quint64 message::getTransmitTime()
{
    // FIXME: переписать нормально
    return (m_length + sizeof(m_SFD) + sizeof(m_length) + sizeof(m_preambule)) / V;
}
