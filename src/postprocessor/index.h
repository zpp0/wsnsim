#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include <QDataStream>
#include <QVector>

class QTemporaryFile;
class QFile;
class QString;
class LogEvent;
class Filter;

class Index : public QWidget
{
    Q_OBJECT
public:
    Index(const QString &fileName, Filter *filter, QWidget *parent = 0);
    ~Index();
    //возвращает адрес массива событий LogEvent или Null если следующей страницы нет
    QVector<LogEvent*> * getNextPage();
    //возвращает адрес массива событий LogEvent или Null если предыдущей страницы нет
    QVector<LogEvent*> * getPrevPage();
    //возвращает адресс массива событий LogEvent
    QVector<LogEvent*> * getCurPage();

    int getCurPageNum() const;
    bool setCurPageNum(int newPage);
    Filter * getFilter() const;

signals:
    void indexChanged();

public slots:
    void createIndex();

private:
    QTemporaryFile* index;
    QFile* log;
    QDataStream indexStream;
    QDataStream logStream;
    Filter * m_pfilter;
    //адреса начала логических страниц в файле индекса
    struct Pages
    {
        //массив адресов
        QVector<quint64> addr;
        //текущая страница
        int curAddr;
    } pages;

    //текущая хранящаяся в памяти страница данных
    QVector<LogEvent*> m_curPage;
};

enum { NumEventsOnPage = 100 };

#endif // INDEX_H
