/***********************************************
 *
 * File: loggableEvent.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <QMap>

#include "event.h"

enum paramType {
    paramType_INT,
    paramType_DOUBLE,
    paramType_STRING,
};
    

class loggableEvent {

public:
    QMap<QString, paramType> types;
    QMap<QString, quint64> intValues;
    QMap<QString, double> doubleValues;
    QMap<QString, QString> stringValues;

};

QDataStream &operator<<(QDataStream &out, const LogEvent &event);
QDataStream &operator>>(QDataStream &in, LogEvent &event);

QTextStream &operator<<(QTextStream &out, const LogEvent &event);
QTextStream &operator>>(QTextStream &in, LogEvent &event);
