#include "filter.h"
#include "logevent.h"
#include <QFile>
#include <QDebug>

Filter::Filter(QObject * parent) : QObject(parent)
{
    for (int i = 0; i < NumOfEventType; i++)
    {
        //первоначально фильтр пропускает все типы события
        m_filter.append(1);
     }
}

bool Filter::verify(const LogEvent &event)
{
    if ( m_filter[event.m_type] == 1 )
    {
        //событие проходит фильтр
        return true;
    }
    return false;
}

void Filter::setFilter(const QVector<quint8> &filter)
{
    //копируем по-элементно что бы знать изменился ли фильтр
    bool filterChangd = false;
    for (int i = 0; i < NumOfEventType; i++)
    {
        if ( m_filter[i] != filter[i] )
        {
            filterChangd = true;
        }
        m_filter[i] = filter[i];
    }
    if ( filterChangd )
    {
        //записываем фильтр в файл настроек
        QFile setting(m_fileSetting);
        qDebug() << m_fileSetting;
        if (!setting.open(QIODevice::WriteOnly))
        {
           //показываем предупреждение пользователю о невозможности создания/записи файла
            qDebug() << tr("Невозможно записать файл настроек");
        }
        else
        {
            QDataStream stream(&setting);
            stream << m_filter;
            setting.close();
        }
        //посылаем сигнал что фильтр изменился
        emit filterChanged();
    }
}

QVector<quint8> Filter::getFilter() const
{
    return m_filter;
}

void Filter::setSettingFile(const QString fileName)
{
    //запоминаем где хранятся настройки
    m_fileSetting = fileName;
    //считавыем настройки фильтра из файла
    QFile setting(m_fileSetting);
    if (setting.open(QIODevice::ReadOnly))
    {
        if (setting.size() != 0)
        {
            QDataStream stream(&setting);
            m_filter.clear();
            stream >> m_filter;
        }
    }
}
