#include <QString>
#include <QStringList>
#include <QDebug>
#include "logevent.h"

#include <iostream>


LogEvent::LogEvent(EventType type)
{
    m_type = type;
    switch(m_type)
    {
    case EventType_NodeOn:
        //узел включился
        m_pNodeOnParam = new NodeOnParam();
        break;
    case EventType_NodeOff:
        //узел выключился (вышел из строя)
        break;
    case EventType_ChangeChanell:
        //смена канала
        m_pChgChnlParam = new ChangeChanellParam();
        break;
    case EventType_RX:
        //прием сообщения
        m_prxEventParam = new RXParam();
        break;
    case EventType_RXProcess:
        //обработка сообщения
        break;
    case EventType_TX:
        //передача сообщения
        m_ptxEventParam = new TXParam();
        break;
    case EventType_TIMER:
        //прерывание по таймеру
        break;
    case EventType_MEASURE:
        //измерение
        m_pmeasureEventParam = new MeasureParam();
        break;
    case EventType_MOVE:
        //перемещение узла
        m_pmoveEventParam = new MoveParam();
        break;
    case EventType_LOGTIME:
        //запись в журнал значения локального таймера узла
        m_plogtimeEventParam = new LogTime();
        break;
    case EventType_CHANGELINK:
        //добавление связи в матрице достижимости
        m_pchangeLinkEventParam = new ChangeLink();
        break;
    default:
        //дествия с пользовательскими типами событий
        break;
    }
}

LogEvent::~LogEvent()
{
    // std::cerr << m_type;
    switch(m_type)
    {
    case EventType_NodeOn:
        //узел включился
        delete m_pNodeOnParam;
        break;
    case EventType_NodeOff:
        //узел выключился
        break;
    case EventType_ChangeChanell:
        //смена канала
        delete this->m_pChgChnlParam;
        break;
    case EventType_RX:
        //прием сообщения
        delete m_prxEventParam;
        break;
    case EventType_RXProcess:
        //обработка узлом принятого сообщения
        break;
    case EventType_TX:
        //передача сообщения
        delete m_ptxEventParam;
        break;
    case EventType_TIMER:
        //прерывание по таймеру
        break;
    case EventType_MEASURE:
        //измерение
        delete m_pmeasureEventParam;
        break;
    case EventType_MOVE:
        //перемещение узла
        delete m_pmoveEventParam;
        break;
    case EventType_LOGTIME:
        //запись в журнал значения локального таймера узла
        delete m_plogtimeEventParam;
        break;
    case EventType_CHANGELINK:
        //добавление связи в матрице достижимости
        delete m_pchangeLinkEventParam;
        break;
    default:
        //дествия с пользовательскими типами событий
        break;
    }
}

QDataStream &operator<<(QDataStream &out, const LogEvent &event)
{
    //записываем в поток общие для всех типов событий параметры
    //(тип события, время наступления, идентификатор узла)
    out << quint8(event.m_type) << event.m_time << event.m_nodeId;

    //а теперь характерные для конкретного события
    switch(event.m_type)
    {
    case EventType_NodeOn:
        //включение узла
        out << event.m_pNodeOnParam->x << event.m_pNodeOnParam->y << event.m_pNodeOnParam->z;
        break;
    case EventType_NodeOff:
        //выключение узла
        break;
    case EventType_ChangeChanell:
        //смена канала
        out << event.m_pChgChnlParam->chanell;
        break;
    case EventType_RX:
        //прием сообщения
        out << event.m_prxEventParam->message << event.m_prxEventParam->RSSI;
        //out << event.m_prxEventParam->senderId << event.m_prxEventParam->message << event.m_prxEventParam->RSSI;
        break;
    case EventType_RXProcess:
        //обработка принятого сообщения
        break;
    case EventType_TX:
        //передача сообщения
        out << event.m_ptxEventParam->message << event.m_ptxEventParam->RSSI;
        break;
    case EventType_TIMER:
        //прерывание по таймеру
        break;
    case EventType_MEASURE:
        //измерение
        out << event.m_pmeasureEventParam->sensorId << event.m_pmeasureEventParam->value;
        break;
    case EventType_MOVE:
        //перемещение узла
        out << event.m_pmoveEventParam->x << event.m_pmoveEventParam->y << event.m_pmoveEventParam->z;
        break;
    case EventType_LOGTIME:
        //запись в журнал значения локального таймера узла
        out << event.m_plogtimeEventParam->localTime;
        break;
    case EventType_CHANGELINK:
        //изменения связи в матрице достижимости
        out << event.m_pchangeLinkEventParam->id2 << event.m_pchangeLinkEventParam->RSSI;
        break;
    default:
        //дествия с пользовательскими типами событий
        break;
    }
    return out;
}

QDataStream &operator>>(QDataStream &in, LogEvent &event)
{
    //считываем параметры события общие для всех типов
    quint8 type;
    in >> type >> event.m_time >> event.m_nodeId;
    event.m_type = EventType(type);

    //а теперь характерные конкретному типу события
    switch (event.m_type)
    {
    case EventType_NodeOn:
        //узел включился
        event.m_pNodeOnParam = new NodeOnParam();
        //для этого события считываем координаты узла
        in >> event.m_pNodeOnParam->x >> event.m_pNodeOnParam->y >> event.m_pNodeOnParam->z;
        break;
    case EventType_NodeOff:
        //узел выключился
        break;
    case EventType_ChangeChanell:
        //смена канала
        event.m_pChgChnlParam = new ChangeChanellParam();
        in >> event.m_pChgChnlParam->chanell;
        break;
    case EventType_RX:
        //прием сообщения
        event.m_prxEventParam = new RXParam();
        in >> event.m_prxEventParam->message >> event.m_prxEventParam->RSSI;
        break;
    case EventType_RXProcess:
        //обработка сообщения
        break;
    case EventType_TX:
        //передача сообщения
        event.m_ptxEventParam = new TXParam();
        in >> event.m_ptxEventParam->message >> event.m_ptxEventParam->RSSI;
        break;
    case EventType_TIMER:
        //прерывание по таймеру
        break;
    case EventType_MEASURE:
        //измерение
        event.m_pmeasureEventParam = new MeasureParam();
        in >> event.m_pmeasureEventParam->sensorId >> event.m_pmeasureEventParam->value;
        break;
    case EventType_MOVE:
        //перемещение узла
        event.m_pmoveEventParam = new MoveParam();
        in >> event.m_pmoveEventParam->x >> event.m_pmoveEventParam->y >> event.m_pmoveEventParam->z;
        break;
    case EventType_LOGTIME:
        //запись в журнал значения локального таймера узла
        event.m_plogtimeEventParam = new LogTime();
        in >> event.m_plogtimeEventParam->localTime;
        break;
    case EventType_CHANGELINK:
        //изменение связи в матрице достижимости
        event.m_pchangeLinkEventParam = new ChangeLink();
        in >> event.m_pchangeLinkEventParam->id2 >> event.m_pchangeLinkEventParam->RSSI;
        break;
    default:
        //дествия с пользовательскими типами событий
        break;
    }
    return in;
}


QTextStream &operator<<(QTextStream &out, const LogEvent &event)
{
    //записываем в поток общие для всех типов событий параметры
    //(тип события, время наступления, идентификатор узла)
    out << quint8(event.m_type) << ";" << event.m_time << ";" << event.m_nodeId << ";";

    //а теперь характерные для конкретного события
    switch(event.m_type)
    {
    case EventType_NodeOn:
        //узел включился
        out << event.m_pNodeOnParam->x << ";"
            << event.m_pNodeOnParam->y << ";" 
            << event.m_pNodeOnParam->z << endl;
        break;
    case EventType_ChangeChanell:
        //смена канала
        out << event.m_pChgChnlParam->chanell << endl;
        break;

    case EventType_RX:
        //прием сообщения
        out << event.m_prxEventParam->message << ";"
            << event.m_prxEventParam->RSSI << endl;
        break;

    case EventType_TX:
        //передача сообщения
        out << event.m_ptxEventParam->message << ";"
            << event.m_ptxEventParam->RSSI << endl;
        break;

    case EventType_MEASURE:
        //измерение
        out << event.m_pmeasureEventParam->sensorId << ";" 
            << event.m_pmeasureEventParam->value << endl;
        break;

    case EventType_MOVE:
        //перемещение узла
        out << event.m_pmoveEventParam->x << ";"
            << event.m_pmoveEventParam->y << ";"
            << event.m_pmoveEventParam->z << endl;
        break;

    case EventType_LOGTIME:
        //запись в журнал значения локального таймера узла
        out << event.m_plogtimeEventParam->localTime << endl;
        break;

    case EventType_CHANGELINK:
        //изменения связи в матрице достижимости
        out << event.m_pchangeLinkEventParam->id2 << ";" << event.m_pchangeLinkEventParam->RSSI << endl;
        break;

    default:
        out << endl;
        //дествия с пользовательскими типами событий
        break;
    }

    return out;
}

QTextStream &operator>>(QTextStream &in, LogEvent &event)
{
    QString line;
    QVector<quint8> body;
    QStringList mes;
    in >> line;
    //получаем список полей, разбивая строку на части согласно обнаруженным символам разделителям
    QStringList fields = line.split(';');

    //полей всегда 14 или более (см.формат)
//    if (fields.size() >= 14)
//    {
//        //считываем параметры общие для всех событий
//        event.m_type = EventType(fields.takeFirst().toInt());
//        event.m_time = fields.takeFirst().toULongLong();
//        event.m_nodeId = fields.takeFirst().toInt();

//        //а теперь конкретные параметры для каждого типа событий
//        switch(event.m_type)
//        {
//        case EventType_HEARDMESSAGE:
//            event.m_pHeardMessageParam = new HeardMessageParam();
//            mes = fields[1].split('-');
//            //начинаем цикл с 1 т.к. первый элемент - длина сообщения (здесь роли не играет)
//            for (int i = 1; i < mes.size(); i++)
//            {
//                body.append(mes[i].toInt());
//            }
//            event.m_pHeardMessageParam->message.setBody(body);
//            break;

//        case EventType_RX:
//            event.m_prxEventParam = new RXParam();
//            event.m_prxEventParam->senderId = fields.takeFirst().toInt();
//            mes = fields.takeFirst().split('-');
//            for (int i = 1; i < mes.size(); i++)
//            {
//                body.append(mes[i].toInt());
//            }
//            event.m_prxEventParam->message.setBody(body);
//            event.m_prxEventParam->RSSI = fields.takeLast().toDouble();
//            break;
//        case EventType_TX:
//            event.m_ptxEventParam = new TXParam();
//            mes = fields[1].split('-');
//            for (int i = 0; i < mes.size(); i++)
//            {
//                body.append(mes[i].toInt());
//            }
//            event.m_ptxEventParam->message.setBody(body);
//            break;

//        case EventType_TIMER:
//            event.m_ptimerEventParam = new TimerParam();
//            break;

//        case EventType_MEASURE:
//            event.m_pmeasureEventParam = new MeasureParam();
//            event.m_pmeasureEventParam->sensorId = fields[2].toInt();
//            event.m_pmeasureEventParam->value = fields[3].toDouble();
//            break;

//        case EventType_MOVE:
//            event.m_pmoveEventParam = new MoveParam();
//            event.m_pmoveEventParam->x = fields[4].toDouble();
//            event.m_pmoveEventParam->y = fields[5].toDouble();
//            event.m_pmoveEventParam->z = fields[6].toDouble();
//            break;

//        case EventType_LOGTIME:
//            event.m_plogtimeEventParam = new LogTime();
//            event.m_plogtimeEventParam->localTime = fields[7].toDouble();
//            break;

//        case EventType_CHANGELINK:
//            event.m_pchangeLinkEventParam = new ChangeLink();
//            event.m_pchangeLinkEventParam->id1 = fields[8].toInt();
//            event.m_pchangeLinkEventParam->id2 = fields[9].toInt();
//            event.m_pchangeLinkEventParam->RSSI = fields[10].toDouble();
//            break;

//        default:
//            break;
//        }
//    }
    //считываем  последний символ перевода строки
    QChar ch;
    in >> ch;
    return in;
}



//определение методов класса message

unsigned short Message::length()
{
    return m_body.size();
}

void Message::setBody(QVector<quint8> &body)
{
    for (int i = 0; i < body.size(); i++)
    {
        m_body.append(body[i]);
    }
}

QDataStream &operator<<(QDataStream &out, const Message &message)
{
    out << message.m_body;
    return out;
}

QDataStream &operator>>(QDataStream &in, Message &message)
{
    in >> message.m_body;
    return in;
}

QTextStream &operator<<(QTextStream &out, const Message &message)
{
    //выводим байты сообщения в виде шестнадцатиричных чисел
    //каждый символ разделен нижним подчеркиванием
    for (int i = 0; i < message.m_body.size(); i++)
    {
        out << hex << message.m_body[i] << '_';
        // out << byte << " ";
    }
    //
    out << dec;

    return out;
}

QTextStream &operator>>(QTextStream &in, Message &message)
{
    //считываем строку сообщения в текстовом виде из потока
    QString line;
    in >> line;
    //разделяем строку на поля согласно разделителям (нижнее подчеркивание)
    QStringList fields = line.split('_');

    //заполняем тело объекта message
    for ( int i = 0; i < fields.size() - 1; i++)
    {
        //каждый элемент - шестнадцатиричное число
        message.m_body.append(fields[i].toUInt(0,16));
    }
    return in;
}
