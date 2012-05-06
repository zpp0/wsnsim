#ifndef TABLEVIEW_H
#define TABLEVIEW_H


#include <QTableWidget>
#include <QVector>

class QScrollBar;
class LogEvent;
class Index;

class TableView : public QTableWidget
{
    Q_OBJECT
public:
    explicit TableView(Index * index, QWidget *parent = 0);

signals:

public slots:
    /*
     Отображает первые 2 страницы журнала
     */
    void slotAddFirstPages();
    /*
     На основе положения вертикальной полосы прокрутки принимает
     решение добавлять ли новую страницу данных в таблицу
     */
    void slotAddPage(int);

private:
    /*
     Добавляет следующую страницу данных в таблицу
     возвращает true в случае успеха, false - если следующая страница отсутствует
     */
    bool addNextPage();
    /*
     Добавляет предыдущую страницу данных в таблицу
     возвращает true в случае успеха, false - если предыдущая страница отсутствует
     */
    bool addPrevPage();

    void addPageDown(QVector<LogEvent*> *events);
    void addPageUp(QVector<LogEvent*> *events);

    //указатель на индекс файла из которого производится чтение
    Index * m_pindex;
    //указатель на вертикальную полосу прокрутки
    QScrollBar * m_pscroll;
    //номер текущей страницы
    int m_pageNumber;
    //признак вверх или вниз последний раз добавлялась страница
    int m_flag;
    //последнее положение полосы прокрутки
    int m_prevValue;
    //позволяет определить какие колонки отображать в таблице на основе
    //настроек фильтра
    QVector<quint8> m_numCol;
};

#endif // TABLEVIEW_H
