#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include <QVector>
#include <QString>

class LogEvent;

class Filter : public QObject
{
    Q_OBJECT
public:
    Filter(QObject * parent = 0);
    //возвращает true, если событие проходит фильтр
    bool verify(const LogEvent &event);
    //устанавливает путь к файлу настройки фильтра
    void setSettingFile(const QString);

    QVector<quint8> getFilter() const;
signals:
    void filterChanged();

public slots:
    void setFilter(const QVector<quint8> &filter);

private:
    //хранит информацию события какого типа интересуют пользователя (по умолчанию - все)
    QVector<quint8> m_filter;
    //путь к файлу настроек фильтра
    QString m_fileSetting;
};

#endif // FILTER_H
