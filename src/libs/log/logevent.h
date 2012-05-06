#ifndef LOGEVENT_H
#define LOGEVENT_H

#include <QDataStream>
#include <QTextStream>
#include <QVector>

class Message
{
public:
    Message(){}
    unsigned short length();
    void setBody(QVector<quint8> &body);

    friend QDataStream &operator<<(QDataStream &out, const Message &message);
    friend QDataStream &operator>>(QDataStream &in, Message &event);

    friend QTextStream &operator<<(QTextStream &out, const Message &message);
    friend QTextStream &operator>>(QTextStream &in, Message &message);
public:
    //тело сообщения
    QVector<quint8> m_body;
};


//стандартные типы событий
enum EventType
{
    EventType_NodeOn,//узел включился
    EventType_NodeOff,//узел выключился (вышел из строя)
    EventType_ChangeChanell, //смена канала
    EventType_RX, //узел принял сообщение по радио
    EventType_RXProcess,//обработка сообщения по радио
    EventType_TX,//передача сообщения по радио
    EventType_TIMER,//прерывание по таймеру
    EventType_MEASURE,//измерение
    EventType_MOVE,//перемещение узла
    EventType_LOGTIME,//запись в журнал локального времени узла
    EventType_CHANGELINK//изменение связи в матрице достижимости
};

enum { NumOfEventType = 11 };
//структуры параметров конкретного события

//параметры события узел включился
struct NodeOnParam
{
    //координаты узла
    double x;
    double y;
    double z;
};

//параметры события узел выключился (вышел из строя)
struct NodeOffParam
{
    //параметров не предусмотренно
};

//параметры события смены канала
struct ChangeChanellParam
{
    //новый номер канала
    quint8 chanell;
};

//параметры события узел услышал сообщение по радио
struct RXParam
{
    //только само сообщение
    Message message;
    //входная мощность
    double RSSI;
};

//параметры события прием сообщения по радио
struct RXProcessParam
{
    //параметров не предусмотренно
    //адрес отправителя
    //quint16 senderId;
    //сообщение
    //Message message;
    //входная мощность
    //double RSSI;
};

//параметры события передача сообщения по радио
struct TXParam
{
    //сообщение
    Message message;
    //выходная мощность
    double RSSI;
};

//параметры события прерывание по таймеру
struct TimerParam
{
};

//параметры события измерение
struct MeasureParam
{
    //идентификатор датчика на узле
    quint16 sensorId;
    //значение измеренной величины
    double value;
};

//параметры события перемещения узла
struct MoveParam
{
    //новые координаты узла
    double x;
    double y;
    double z;
};

//параметры события запись в журнал значения таймера
struct LogTime
{
    //локальное время таймера
    quint64 localTime;
};

//параметры события изменения связи в матрице достижимости
struct ChangeLink
{
    //идентификатор узла с которым изменилась связь
    quint16 id2;
    //мощность узла
    double RSSI;
};



class LogEvent
{
public:
    LogEvent(EventType type);
    LogEvent(){}
    ~LogEvent();

    //тип события
    EventType m_type;
    //идентификатор узла
    quint16 m_nodeId;
    //время наступления события
    quint64 m_time;
    union
    {
        NodeOnParam *m_pNodeOnParam;
        NodeOffParam *m_pNodeOffParam;
        ChangeChanellParam *m_pChgChnlParam;
        RXParam *m_prxEventParam;
        RXProcessParam *m_pRXProcessParam;
        TXParam *m_ptxEventParam;
        TimerParam *m_ptimerEventParam;
        MeasureParam *m_pmeasureEventParam;
        MoveParam *m_pmoveEventParam;
        LogTime *m_plogtimeEventParam;
        ChangeLink *m_pchangeLinkEventParam;
    };
};

QDataStream &operator<<(QDataStream &out, const LogEvent &event);
QDataStream &operator>>(QDataStream &in, LogEvent &event);

QTextStream &operator<<(QTextStream &out, const LogEvent &event);
QTextStream &operator>>(QTextStream &in, LogEvent &event);

#endif // LOGEVENT_H
