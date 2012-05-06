#ifndef PROCESSESPAGE_H
#define PROCESSESPAGE_H

// #include "processParamsPage.h"
#include "nodeTypeParamsPage.h"

#include <QHash>

#include <QGroupBox>
#include <QTableWidgetItem>

#include "dataParams.h"

namespace Ui {
    class ProcessesPage;
}

class ProcessesPage : public QGroupBox
{
    Q_OBJECT

public:
    ProcessesPage();
    virtual ~ProcessesPage();

    // обраобтчик события добавлена страница с параметрами типа узлов
    // сообщает этой странице об имеющихся в среде процессах, для определения состава датчиков
    void nodeTypeParamsPageAdded(NodeTypeParamsPage *pnt_new);

    // удаление процесса
    void removeProcesses();

    // полечение массива параметров процессов
    QVector<processParams*> getParams();
    // запись параметров процессов из массива
    void setParams(QVector<processParams*> processes);
    
private:
    Ui::ProcessesPage *m_ui;

    // хеш массив страниц параметров процессов и их имен
    // QHash <QString, ProcessParamsPage*> m_h_ptpPages;

    // создание строки с параметрами процесса в таблице
    void createProcessRow(QString pt_name);
    
signals:
    // сигналы

    // процесс создан
    void processAdded(QString name);
    // процесс удален
    void processRemoved(QString name);
    // единицы измерений процесса изменены
    void processUnitChanged(quint8 processID, QString units);
    void processNameChanged(quint8 processID, QString newName);

private slots:
    // обработчики событий
    
    // изменены единицы измерений процесса с именем pt_name
    void unitsChanged(QTableWidgetItem* item);
    void nameChanged(QTableWidgetItem* item);
    // нажата кнопка выбора файла
    void openLuaFile();
    // нажата кнопка открытия файла во внешнем редакторе
    void editLuaFile();
    
    // добавлен процесс
    void addProcess(processParams* params);
    void addProcess(QString pt_name);
    void addProcess();
    
    // удален процесс
    void removeProcess(int row);
    // вызвано контекстное меню в таблице
    void customContextMenuRequested(const QPoint &p);
    // текст в строке названия процесса изменен
    void textChanged(QString text);
};

#endif // PROCESSESPAGE_H
