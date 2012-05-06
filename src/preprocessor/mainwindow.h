#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QHash>
#include <QLabel>

// #include <QUndoStack>

#include "projectPage.h"
#include "envPage.h"
#include "nodeTypesPage.h"
#include "nodeTypeParamsPage.h"
#include "nodesPage.h"
#include "processesPage.h"
// #include "processParamsPage.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();
    
private:

    // добавление страницы в список страниц
    void addPage(QString p_name, QWidget *page, QTreeWidgetItem* parent = 0);
    void addPage(QTreeWidgetItem* ti_page, QWidget *page);

    // удаление страницы из списка страниц
    void removePage(QWidget *page);

    // создание элемента дерева
    QTreeWidgetItem* addTiWidget(QString name, QTreeWidgetItem* parent = 0);

    // запись данных в XML
    void saveXml();
    // чтение данных из XML
    void loadXml();
    
    // метод возврашает параметры симулятора
    simulatorParams* getParams();
    
    Ui::MainWindow *m_ui;

    // массив страниц
    QHash <QTreeWidgetItem*, QWidget*> m_h_pages;

    // путь к файлу проекта
    QString m_projectFileName;

    // стандартные страницы
    // проект
    ProjectPage *m_p_project;
    // среда
    EnvPage *m_p_env;
    // типы узлов
    NodeTypesPage *m_p_nodeTypes;
    // узлы
    NodesPage *m_p_nodes;
    // процессы
    ProcessesPage *m_p_processes;

    // объекты дерева, к ним привязываюся обекты страниц
    // проект
    QTreeWidgetItem *m_ti_project;
    // среда
    QTreeWidgetItem *m_ti_env;
    // процессы
    QTreeWidgetItem *m_ti_processes;
    // типы узлов
    QTreeWidgetItem *m_ti_nodeTypes;
    // узлы
    QTreeWidgetItem *m_ti_nodes;

    QLabel* m_l_projectName;
    
    // версия
    static const quint16 m_version = 1;
    
    // QUndoStack *undostack;
    
private slots:
    // события
    // нажата кнопка сохранить
    void actionSave();
    // сохранить как
    void actionSaveAs();
    // открыть проект
    void actionOpen();
    // выйти
    void actionQuit();

public slots:
    // пользователь перешел на другую страницу
    void changePage(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    // добавлен тип узлов
    void nodeTypeAdded(NodeTypeParamsPage *nt_page, QString name);
    // удален тип узлов
    void nodeTypeRemoved(NodeTypeParamsPage *nt_page, QString name);
    // добавлен процесс
    void processAdded(QString name);
    // удален процесс
    void processRemoved(QString name);
};

#endif // MAINWINDOW_H
