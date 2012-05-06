#include <QtGui>
#include <QDataStream>
#include <QFile>
#include <QTemporaryFile>
#include "index.h"
#include "filter.h"
#include "logevent.h"


Index::Index(const QString &fileName, Filter *filter, QWidget *parent) : QWidget(parent)
{
    //сохраняем указатель на фильтр событий
    m_pfilter = filter;
    //в начальный момент файлы лога и индекса не инициализированы
    log = 0;
    index = 0;

    //изменение фильтра должно вызываеть построение нового индекса
    connect(m_pfilter, SIGNAL(filterChanged()), SLOT(createIndex()));

    //открываем файл журнала
    log = new QFile(fileName);

    if (!log->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Viewer"), tr("Невозможно прочесть файл журнала %1:\n%2.")
                             .arg(log->fileName()).arg(log->errorString()));
    }
    else
    {
        logStream.setDevice(log);

        //создаем индекс
        createIndex();
    }
}

Index::~Index()
{
    delete index;
    delete log;
}

void Index::createIndex()
{
    if (index != NULL)
    {
        delete index;
    }
    //открываем файл индекса
    index = new QTemporaryFile;
    if ( !index->open() )
    {
        QMessageBox::warning(this, tr("DataViewer"), tr("Невозможно создать файл индекса %1:\n%2.")
                             .arg(index->fileName()).arg(index->errorString()));
        return;
    }
    //открываем индекс как поток
    indexStream.setDevice(index);

    quint64 position;
    quint64 indexPos;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    //номер события в файле индекса
    int i = 0;

    //создаем индикатор прогресса
    QProgressDialog  progress(tr("Создание индекса журнала..."), "", 0, log->size(), this);
    //кнопка отмены не нужна
    progress.setCancelButton(NULL);
    //делаем окно индикатора модальным
    progress.setModal(true);
    progress.show();

    //считываем данные из журнала
    while ( !logStream.atEnd() )
    {
        //создаем объект в цикле что бы не было утечки памяти
        LogEvent event;
        //узнаем адрес события в файле журнала
        position = log->pos();
        logStream >> event;

        //устанавливаем значение индикатора
        progress.setValue(position);

        //проверяем событие
        if ( m_pfilter->verify(event) )
        {
            indexPos = index->pos();
            //записываем адрес события в индекс
            indexStream << position;
            //i - номер события, адрес которого записывается в индекс
            if ( i % NumEventsOnPage == 0 )
            {
                //запоминаем адрес начала страницы индекса
                pages.addr.append(indexPos);
            }
            i++;
        }
        //выполнить все накопившиеся события
        qApp->processEvents();
    }

    QApplication::restoreOverrideCursor();
    //устанавливаем адрес текущей страницы - 0
    pages.curAddr = 0;
    //высылаем сигнал что индекс изменился
    emit indexChanged();
}

QVector<LogEvent*> * Index::getCurPage()
{
    //освобождаем память от текущей страницы
    for (int i = 0; i < m_curPage.size(); i++)
    {
        delete m_curPage[i];
    }
    m_curPage.clear();



    LogEvent * pevent = 0;
    quint64 position;
    //устанавливаем индекс на начало нужной страницы
    index->seek(pages.addr[pages.curAddr]);

    //считываем следующую страницу
    for (int i = 0; i < NumEventsOnPage && !indexStream.atEnd(); i++)
    {
        //выделяем память под очередное событие
        pevent = new LogEvent;
        //считываем из индекса адрес очередного события в файле журнала
        indexStream >> position;
        //устанавливаем позицию журнала на нужный адрес
        log->seek(position);
        //считываем событие
        logStream >> *pevent;
        //запоминаем его
        m_curPage.append(pevent);
    }

    return &m_curPage;
}

QVector<LogEvent*> * Index::getNextPage()
{
    //делаем текущей следующую страницу
    if ( pages.curAddr < pages.addr.size() - 1)
    {
        pages.curAddr++;
        //считываем теперь уже текущую страницу
        getCurPage();
        return &m_curPage;

    }
    return NULL;

}

QVector<LogEvent*> *Index::getPrevPage()
{
    //делаем текущей предыдущую страницу
    if ( pages.curAddr > 0)
    {
        pages.curAddr--;
        //считываем теперь уже текущую страницу
        getCurPage();
        return &m_curPage;
    }
    return NULL;
}


int Index::getCurPageNum() const
{
    return pages.curAddr;
}

bool Index::setCurPageNum(int newPage)
{
    if (newPage >= 0 && newPage <= (pages.addr.size() - 1) )
    {
        pages.curAddr = newPage;
        return true;

    }
    return false;
}

Filter * Index::getFilter() const
{
    return m_pfilter;
}
