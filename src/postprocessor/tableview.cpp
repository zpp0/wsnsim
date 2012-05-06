#include <QtGui>
#include <QScrollBar>
#include <QVector>
#include "logevent.h"
#include "index.h"
#include "filter.h"
#include "tableview.h"
#include <QDebug>

TableView::TableView(Index * index, QWidget *parent) : QTableWidget(parent)
{
    //сохраняем указатель на индекс файла журнала
    m_pindex = index;

    //получаем указатель на вертикальную полосу прокрутки
    m_pscroll = verticalScrollBar();
    //соединяем сигнал изменения положения полосы прокрутки со слотом добавления новой страницы
    connect(m_pscroll, SIGNAL(valueChanged(int)), this, SLOT(slotAddPage(int)));

    //соединяем сигнал изменения индекса с слотом отображения начала журнала
    connect(m_pindex, SIGNAL(indexChanged()), this, SLOT(slotAddFirstPages()));

    //отображаем начало журнала
    slotAddFirstPages();
}

void TableView::slotAddFirstPages()
{
    //очищаем таблицу
    clear();
    setColumnCount(0);
    setRowCount(0);

    //получаем фильтр для настройки отображения колонок в таблице
    QVector<quint8> filter = m_pindex->getFilter()->getFilter();
    //получаем номера полей которые необходимы для отображения событий проходящих фильтр

    //какие поля необходимы для отображения конкретного события
    //подробно смотри документацию
    quint8 matrix[8][14] =
                {
                    {1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},//узел принял сообщение по радио
                    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},//прием сообщения по радио
                    {1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},//передача сообщения по радио
                    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},//прерывание по таймеру
                    {1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},//измерение
                    {1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},//перемещение узла
                    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},//запись в журнал значения таймера
                    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1} //изменение связи в матрице достижимости
                };
    //цикл по полям фильтра
    quint8 vector[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 8; i++)
    {
        //если событие нужно отображать
        if ( filter[i] )
        {
            for (int j = 0; j < 14; j++)
            {
                if ( matrix[i][j] )
                {
                    //запоминаем что это поле необходимо для отображения события
                    vector[j] = 1;
                }
            }
        }
    }

    m_numCol.clear();
    //теперь получаем только номера необходимых полей
    for (int i = 0; i < 14; i++)
    {
        if ( vector[i] )
        {
            m_numCol.append(i);
        }
    }

    //настраиваем таблицу
    //количество столбцов
    setColumnCount(m_numCol.size());
    //устанавливаем горизонтальный заголовок таблицы
    QStringList labels;
    labels << tr("Тип\nсобытия") << tr("Время") << tr("Идентификатор\nузла")
            << tr("Адрес\nотправителя") << tr("Сообщение") << tr("Идентификатор\nдатчика\nна узле")
            << tr("Значение\nизмеренной\nвеличины") << tr("Новая\nкоордината\nX") << tr("Новая\nкоордината\nY")
            << tr("Новая\nкоордината\nZ")<< tr("Локальное\nвремя\nтаймера\nузла") << tr("Идентификатор\ni-ого\nузла")
            << tr("Идентификатор\nj-ого\nузла") << tr("RSSI");

    for (int i = 0; i < m_numCol.size();i++)
    {
        QTableWidgetItem * item = new QTableWidgetItem(labels[m_numCol[i]]);
        setHorizontalHeaderItem(i, item);

    }

    //получаем первые две страницы журнала и отображаем их
    for (int i = 0; i < 2; i++)
    {
        if ( m_pindex->setCurPageNum(i) )
        {
            QVector<LogEvent*> * events = m_pindex->getCurPage();
            //отображаем данные в таблице
            for (int i = 0; i < events->size(); i++)
            {
                qDebug() << "klsjd";
                QString str;
                QTextStream stream(&str);
                //получаем текстовое предствление события
                stream << *(events->at(i));
                //разбиваем его на поля используя разделитель ';'
                QStringList fields = str.split(";");
                //добавляем новую строку в таблицу
                qDebug() << rowCount();
                insertRow(rowCount());
                for (int i = 0; i < m_numCol.size(); i++)
                {
                    //новая ячейка
                    QTableWidgetItem * newItem = new QTableWidgetItem(fields[m_numCol[i]]);
                    //устанавливаем ее в текущей строке
                    setItem(rowCount() - 1, i, newItem);
                }
//                for (int column = 0; column < 14; column++)
//                {
//                    //новая ячейка
//                    QTableWidgetItem * newItem = new QTableWidgetItem(fields.takeFirst());
//                    //устанавливаем ее в текущей строке
//                    setItem(rowCount() - 1, column, newItem);
//                }
            }
        }
    }
    m_pageNumber = 1;
    m_flag = 0;
    m_prevValue = -1;
}


bool TableView::addNextPage()
{
    //получаем страницу событий
    QVector<LogEvent*> * events = m_pindex->getCurPage();
    //если следующая страница есть
    if ( events != NULL)
    {
        //отображаем данные в таблице
        for (int i = 0; i < events->size(); i++)
        {
            QString str;
            QTextStream stream(&str);
            //получаем текстовое предствление события
            stream << *(events->at(i));
            //разбиваем его на поля используя разделитель ';'
            QStringList fields = str.split(";");
            //добавляем новую строку в таблицу
            insertRow(rowCount());
            for (int i = 0; i < m_numCol.size(); i++)
            {
                //новая ячейка
                QTableWidgetItem * newItem = new QTableWidgetItem(fields[m_numCol[i]]);
                //устанавливаем ее в текущей строке
                setItem(rowCount() - 1, i, newItem);
            }
//            for (int column = 0; column < 14; column++)
//            {
//                //новая ячейка
//                QTableWidgetItem * newItem = new QTableWidgetItem(fields.takeFirst());
//                //устанавливаем ее в текущей строке
//                setItem(rowCount() - 1, column, newItem);
//            }
        }
        return true;
    }
    return false;
}

void TableView::slotAddPage(int value)
{
    //указатель на страницу событий в памяти
    QVector<LogEvent*> * events;
    //признак успешной загрузки страницы
    bool pageLoaded = false;
    //максимальное значение положения полосы прокрутки
    int maxValue = m_pscroll->maximum();
    //минимальное
    int minValue = m_pscroll->minimum();
    //количество позиций на странице
    int pageStep = m_pscroll->pageStep();

    qDebug() << m_pageNumber;

    //если положение полосы прокрутки > maxValue - pageStep
    if (value >= maxValue - pageStep && m_prevValue < maxValue - pageStep)
    {
        //если последний раз страница добавлялась вниз
        if ( m_flag == 0 )
        {
            //загружаем следующую страницу
            pageLoaded = m_pindex->setCurPageNum(m_pageNumber + 1);
            if (pageLoaded)
            {
                m_pageNumber++;
            }
        }
        else
        {
            m_flag = 0;
            //загружаем  страницу за номером m_pageNumber + 2
            pageLoaded = m_pindex->setCurPageNum(m_pageNumber + 2);
            if (pageLoaded)
            {
                m_pageNumber += 2;
            }
        }

        //страница загружена
        if ( pageLoaded )
        {
            //получаем данные
            events = m_pindex->getCurPage();
            //и отображаем их внизу таблицы
            //разъединяем сигнал изменения положения полосы прокрутки и слотом добавления событий
            disconnect(m_pscroll, SIGNAL(valueChanged(int)), this, SLOT(slotAddPage(int)));
            //добавляем страницу
            addPageDown(events);
            //устанавливаем полосу прокрутки на конец предпоследней страницы событий
            m_pscroll->setSliderPosition(maxValue / 2);
            m_prevValue = maxValue / 2;
            //соединяем вновь
            connect(m_pscroll, SIGNAL(valueChanged(int)), this, SLOT(slotAddPage(int)));
            return;
        }
    }
    else if ( value <= minValue + pageStep && m_prevValue > minValue + pageStep)
    {
        //если последний раз страница добавлялась вверх
        if ( m_flag == 1)
        {
            //загружаем предыдущую страницу
            pageLoaded = m_pindex->setCurPageNum(m_pageNumber - 1);
            if (pageLoaded)
            {
                m_pageNumber--;
            }
        }
        else
        {
            m_flag = 1;
            //загружаем страницу с номером на 2 меньше чем текущая
            pageLoaded = m_pindex->setCurPageNum(m_pageNumber - 2);
            if (pageLoaded)
            {
                m_pageNumber -= 2;
            }
        }

        //страница загружена
        if ( pageLoaded )
        {
            //получаем данные
            events = m_pindex->getCurPage();

            disconnect(m_pscroll, SIGNAL(valueChanged(int)), this, SLOT(slotAddPage(int)));
            //отображаем их вверху таблицы
            addPageUp(events);
            //устанавливаем полосу прокрутки на стык страниц
            m_pscroll->setSliderPosition(maxValue / 2);
            m_prevValue = maxValue / 2;
            connect(m_pscroll, SIGNAL(valueChanged(int)), this, SLOT(slotAddPage(int)));
            return;
        }
    }
    //сохраняем текущее положение полосы прокрутки
    m_prevValue = value;
}

void TableView::addPageDown(QVector<LogEvent *> *events)
{
    //если страница есть
    if ( events != NULL)
    {
        //отображаем данные в таблице
        for (int i = 0; i < events->size(); i++)
        {
            QString str;
            QTextStream stream(&str);
            //получаем текстовое предствление события
            stream << *(events->at(i));
            //разбиваем его на поля используя разделитель ';'
            QStringList fields = str.split(";");
            //добавляем новую строку в таблицу
            insertRow(rowCount());
            //номер события в журнале
            //номер страницы уже был увеличен на 1
            QTableWidgetItem * newItem = new QTableWidgetItem(QString::number((m_pageNumber - 1) * NumEventsOnPage + i + 1));
            setVerticalHeaderItem(rowCount() - 1, newItem);

            for (int i = 0; i < m_numCol.size(); i++)
            {
                //новая ячейка
                QTableWidgetItem * newItem = new QTableWidgetItem(fields[m_numCol[i]]);
                //устанавливаем ее в текущей строке
                setItem(rowCount() - 1, i, newItem);
            }
//            for (int column = 0; column < 14; column++)
//            {
//                //новая ячейка
//                QTableWidgetItem * newItem = new QTableWidgetItem(fields.takeFirst());
//                //устанавливаем ее в текущей строке
//                setItem(rowCount() - 1, column, newItem);
//            }
            //удаляем первую строку таблицы для экономии памяти
            removeRow(0);
        }
    }
}

void TableView::addPageUp(QVector<LogEvent *> *events)
{
    //если страница есть
    if ( events != NULL)
    {
        //отображаем данные в таблице
        for (int i = events->size() - 1; i >= 0; i--)
        {
            QString str;
            QTextStream stream(&str);
            //получаем текстовое предствление события
            stream << *(events->at(i));
            //разбиваем его на поля используя разделитель ';'
            QStringList fields = str.split(";");
            //добавляем новую строку в таблицу
            insertRow(0);
            //номер события в журнале
            QTableWidgetItem * newItem = new QTableWidgetItem(QString::number(m_pageNumber * NumEventsOnPage + i + 1));
            setVerticalHeaderItem(0, newItem);

            for (int i = 0; i < m_numCol.size(); i++)
            {
                //новая ячейка
                QTableWidgetItem * newItem = new QTableWidgetItem(fields[m_numCol[i]]);
                //устанавливаем ее в текущей строке
                setItem(0, i, newItem);
            }

//            for (int column = 0; column < 14; column++)
//            {
//                //новая ячейка
//                QTableWidgetItem * newItem = new QTableWidgetItem(fields.takeFirst());
//                //устанавливаем ее в текущей строке
//                setItem(0, column, newItem);
//            }
            //удаляем последнюю строку таблицы для экономии памяти
            removeRow(rowCount() - 1);
        }
    }
}

