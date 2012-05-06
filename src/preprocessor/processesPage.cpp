#include <QTableWidgetItem>
#include <QMenu>
#include <QFileDialog>

#include <QDesktopServices>

#include "processesPage.h"
#include "ui_processesPage.h"

#include "dataParams.h"

ProcessesPage::ProcessesPage() :
    m_ui(new Ui::ProcessesPage)
{
    // инициализируем интерфейс
    m_ui->setupUi(this);

    // блокируем кнопку добавления узлов
    m_ui->b_addProcess->setDisabled(true);

    m_ui->t_processes->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    
    // сигналы

    // нажата кнопка добавления процесса
    connect(m_ui->b_addProcess, SIGNAL(clicked()),
            this, SLOT(addProcess()));

    // изменено имя процесса
    connect(m_ui->le_processName, SIGNAL(textChanged(QString)),
            this, SLOT(textChanged(QString)));

    connect(m_ui->t_processes, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(unitsChanged(QTableWidgetItem*)));

    connect(m_ui->t_processes, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(nameChanged(QTableWidgetItem*)));
    
    // вызвано контекстное меню в таблице
    connect(m_ui->t_processes, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(customContextMenuRequested(const QPoint &)));

    // нажата кнопка "открыть файл"
    connect(m_ui->tb_fileLuaProcesses, SIGNAL(clicked()),
             this, SLOT(openLuaFile()));

    // нажата кнопка "редактировать файл"
    connect(m_ui->tb_fileLuaProcessesEdit, SIGNAL(clicked()),
             this, SLOT(editLuaFile()));
}

ProcessesPage::~ProcessesPage()
{
    delete m_ui;
}

void ProcessesPage::createProcessRow(QString pt_name)
{
    // создаем ячейки для отображения данных о типе процесса в таблице
    // имя процесса
    QTableWidgetItem *ti_pt_name = new QTableWidgetItem(pt_name);
    // единицы измерений
    QTableWidgetItem *ti_pt_units = new QTableWidgetItem();
    // комментарий
    QTableWidgetItem *ti_pt_comment = new QTableWidgetItem();

    // получаем количество процессов
    int rowCount = m_ui->t_processes->rowCount();
    // увеличиваем количество строк на 1
    m_ui->t_processes->setRowCount(rowCount + 1);

    // добавляем в новую строку известные данные о типе процесса
    m_ui->t_processes->setItem(rowCount, 0, ti_pt_name);
    m_ui->t_processes->setItem(rowCount, 1, ti_pt_units);
    m_ui->t_processes->setItem(rowCount, 2, ti_pt_comment);

    // очищаем поля для ввода
    m_ui->le_processName->clear();
}
    
void ProcessesPage::addProcess()
{
    // создан новый тип процесса
    // получаем имя типа процесса
    QString pt_name = m_ui->le_processName->text();
    // qDebug() << pt_name;
    
    createProcessRow(pt_name);
    // инициализируем событие "добавлен процесс"
    emit processAdded(pt_name);
}

void ProcessesPage::addProcess(QString pt_name)
{
    // создан новый тип процесса

    createProcessRow(pt_name);
    
    // инициализируем событие "добавлен процесс"
    emit processAdded(pt_name);
}

void ProcessesPage::addProcess(processParams* params)
{
    // создан новый тип процесса

    QString pt_name;
    if (params->name != NULL)
        pt_name = params->name;

    createProcessRow(pt_name);

    if (params->name != NULL)
        m_ui->t_processes->item(params->processID, 0)->setText(params->name);
    if (params->units != NULL)
        m_ui->t_processes->item(params->processID, 1)->setText(params->units);
    if (params->comment != NULL)
        m_ui->t_processes->item(params->processID, 2)->setText(params->comment);

    // инициализируем событие "добавлен процесс"
    emit processAdded(pt_name);
}

void ProcessesPage::textChanged(QString text)
{
    // текст в строке имени типа добавляемого процесса редактировался

    // блокируем кнопку "добавить", если имя типа добавляемого процесса или единицы измерений не заданы
    if ((text == "")
        || (m_ui->t_processes->findItems(text,Qt::MatchFixedString).size() > 0))
        m_ui->b_addProcess->setEnabled(false);
    else
        m_ui->b_addProcess->setEnabled(true);
}

void ProcessesPage::nameChanged(QTableWidgetItem* item)
{
    // изменены единицы измерений

    if (m_ui->t_processes->column(item) == 0) {

        int processID = m_ui->t_processes->row(item);
        QString pt_name = item->text();

        emit processNameChanged(processID, pt_name);
    }
}

void ProcessesPage::unitsChanged(QTableWidgetItem* item)
{
    // изменены единицы измерений

    if (m_ui->t_processes->column(item) == 1) {

        int processID = m_ui->t_processes->row(item);
        QString units = item->text();

        emit processUnitChanged(processID, units);
    }
}

void ProcessesPage::nodeTypeParamsPageAdded(NodeTypeParamsPage *pnt_new)
{
    
    for (int i = 0; i < m_ui->t_processes->rowCount(); i++) {
        QString pt_name = m_ui->t_processes->item(i, 0)->text();
        QString pt_units = m_ui->t_processes->item(i, 1)->text();
        
        // выдаем датчики типу узлов
        pnt_new->processAdded(pt_name);
        // выдаем единицы измерений датчиков
        pnt_new->processUnitChanged(i, pt_units);
    }

    // событие изменены единицы измерений
    connect(this, SIGNAL(processUnitChanged(quint8, QString)),
            pnt_new, SLOT(processUnitChanged(quint8, QString)));

    // событие добавлен тип процессов
    connect(this, SIGNAL(processAdded(QString)),
            pnt_new, SLOT(processAdded(QString)));

    // событие удален тип процессов
    connect(this, SIGNAL(processRemoved(QString)),
            pnt_new, SLOT(processRemoved(QString)));

    connect(this, SIGNAL(processNameChanged(quint8, QString)),
            pnt_new, SLOT(processNameChanged(quint8, QString)));

}

void ProcessesPage::customContextMenuRequested(const QPoint &p)
{
    // обработчик события вызвано контестное меню в таблице
    
    // создаем объект меню
    QMenu menu(m_ui->t_processes);
    // добавляем в него элемент Remove
    menu.addAction(m_ui->actionRemove);

    // получаем ячейку, над которой пользователь вызвал меню
    QTableWidgetItem *ti_pt = m_ui->t_processes->itemAt(p);

    // если это не ячейка
    if (ti_pt == NULL)
        // ничего не делаем
        return;

    // пользователь нажимает на кнопку в меню, получаем действие, которое он хочет выполнить
    QAction *a = menu.exec(mapToGlobal(p));
    
    // иначе получаем номер строки этой ячейки
    int row = m_ui->t_processes->row(ti_pt);

    // если выбрано действие remove
    if (a == m_ui->actionRemove)
        // удаляем строку
        removeProcess(row);
}

void ProcessesPage::removeProcess(int row)
{
    // удаление процесса

    // вытаскиваем имя удаляемого процесса
    QString pt_name = m_ui->t_processes->item(row, 0)->text();
    
    // удаляем строку с параметрами процесса из таблицы
    m_ui->t_processes->removeRow(row);

    // создаем событие процесс удален
    emit processRemoved(pt_name);
}

void ProcessesPage::removeProcesses()
{
    // удаление всех процессов
    // пока есть строки в таблице
    while (m_ui->t_processes->rowCount() != 0)
        // удаляем верхний процесс из таблицы
        removeProcess(0);
}

void ProcessesPage::openLuaFile()
{
    // открываем диалог для выбора файла
    // получаем путь к файлу
    QString text = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                "", "Lua Script files (*.lua)");

    // записываем имя в соответствующее поле
    m_ui->le_fileLuaProcesses->setText(text);
}

void ProcessesPage::editLuaFile()
{
    // открываем файл во внешнем реакторе
    QDesktopServices::openUrl(m_ui->le_fileLuaProcesses->text());
}

QVector<processParams*> ProcessesPage::getParams()
{
    QVector<processParams*> processes;

    for (int i = 0; i < m_ui->t_processes->rowCount(); i++) {
        processParams* params = new processParams();
        params->processID = i;
        // название процесса
        params->name = m_ui->t_processes->item(i, 0)->text();
        // единицы измерений
        params->units = m_ui->t_processes->item(i, 1)->text();
        // комментарий
        params->comment = m_ui->t_processes->item(i, 2)->text();
        // файл сприпта lua
        params->luaScript = m_ui->le_fileLuaProcesses->text();

        processes.push_back(params);
    }

    // возвращаем результат
    return processes;
}

void ProcessesPage::setParams(QVector<processParams*> processes)
{
    if (processes.size() > 0) {
        // для всех параметров процессов из массива
        for (int i = 0; i < processes.size(); i++)
            if (processes[i] != NULL)
                // создаем процесс с параметрами из массива
                addProcess(processes[i]);
    
        if (processes[0] != NULL)
            m_ui->le_fileLuaProcesses->setText(processes[0]->luaScript);
    }
}
