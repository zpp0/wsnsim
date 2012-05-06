#include <QFileDialog>

#include <QMessageBox>
#include <QStatusBar>
#include <QLabel>

#include <QTextCodec>
#include <QString>

#include "xml.h"
#include "dataParams.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() :
    QMainWindow(),
    m_ui(new Ui::MainWindow)
{
    // инициализируем интерфейс
    m_ui->setupUi(this);

    // устанавливаем кодировки для строк
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

    // FUCK! There is no another way to remove 10px title bar
    QWidget* treeTitle = new QWidget(this);
    m_ui->dockWidget->setTitleBarWidget(treeTitle);
    
    // создаем объекты стандартных страниц
    // проект
    m_p_project = new ProjectPage();
    // среда
    m_p_env = new EnvPage();
    // процессы
    m_p_processes = new ProcessesPage();
    // типы узлов
    m_p_nodeTypes = new NodeTypesPage();
    // узлы
    m_p_nodes = new NodesPage();

    // создаем элементы дерева стандартных страниц
    // проект
    m_ti_project = addTiWidget("Проект");
    // размеры
    m_ti_env = addTiWidget("Размеры");
    // процессы
    m_ti_processes = addTiWidget("Процессы");
    // типы узлов
    m_ti_nodeTypes = addTiWidget("Типы узлов");
    // узлы
    m_ti_nodes = addTiWidget("Расположение узлов");

    // создаем стандартные страницы
    addPage(m_ti_project, m_p_project);
    addPage(m_ti_env, m_p_env);
    addPage(m_ti_processes, m_p_processes);
    addPage(m_ti_nodeTypes, m_p_nodeTypes);
    addPage(m_ti_nodes, m_p_nodes);

    // путь к файлу исходных данных
    m_projectFileName = "";

    // undostack = new QUndoStack ();

    m_l_projectName = new QLabel("New project");
    
    m_l_projectName->setFrameShape(QFrame::NoFrame);
    m_l_projectName->setFrameShadow(QFrame::Plain);
    
    m_ui->statusBar->addPermanentWidget(m_l_projectName);
    
    // сигналы

    // сигнал о смене размеров среды обрабатывает страница с узлами
    connect(m_p_env, SIGNAL(envSizeChanged(double, int)),
            m_p_nodes, SLOT(envSizeChanged(double, int)));

    // сигнал о добавлении процесса обрабатывает основное окно
    connect(m_p_processes, SIGNAL(processAdded(QString)),
            this, SLOT(processAdded(QString)));

    // сигнал об удалении процесса обрабатывает основное окно
    connect(m_p_processes, SIGNAL(processRemoved(QString)),
            this, SLOT(processRemoved(QString)));

    // сигнал о добавлении типа узла обрабатывает основное окно
    connect(m_p_nodeTypes, SIGNAL(nodeTypeAdded(NodeTypeParamsPage*, QString)),
            this, SLOT(nodeTypeAdded(NodeTypeParamsPage*, QString)));

    // сигнал об удалении типа узла обрабатывает основное окно
    connect(m_p_nodeTypes, SIGNAL(nodeTypeRemoved(NodeTypeParamsPage*, QString)),
            this, SLOT(nodeTypeRemoved(NodeTypeParamsPage*, QString)));

    // сигнал о добавлении типа узла обрабатывает страница с узлами
    connect(m_p_nodeTypes, SIGNAL(nodeTypeAdded(NodeTypeParamsPage*, QString)),
            m_p_nodes, SLOT(nodeTypeAdded(NodeTypeParamsPage*, QString)));

    // сигнал об удалении типа узла обрабатывает страница с узлами
    connect(m_p_nodeTypes, SIGNAL(nodeTypeRemoved(NodeTypeParamsPage*, QString)),
            m_p_nodes, SLOT(nodeTypeRemoved(NodeTypeParamsPage*, QString)));

    // сигнал о нажатии на элемент дерева обрабатывает основное окно
    connect(m_ui->tr_options, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(changePage(QTreeWidgetItem*, QTreeWidgetItem*)));

    // сигнал нажатия кнопок обрабатывает основное окно
    connect(m_ui->actionSave, SIGNAL(triggered()),
            this, SLOT(actionSave()));

    connect(m_ui->actionOpen, SIGNAL(triggered()),
            this, SLOT(actionOpen()));
    
    connect(m_ui->actionSaveAs, SIGNAL(triggered()),
            this, SLOT(actionSaveAs()));

    connect(m_ui->actionQuit, SIGNAL(triggered()),
            this, SLOT(actionQuit()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::changePage(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    // нажат элемент дерева - надо показать соответствующую страницу

    // если элемент дерева нажат повторно, то просто выходим
    if (current == previous)
        return;

    // иначе получаем указатель на страницу и показываем ее
    m_ui->pages->setCurrentWidget(m_h_pages.value(current));
}

void MainWindow::nodeTypeAdded(NodeTypeParamsPage *n_page, QString name)
{
    // добавлен тип узлов

    // создаем страницу в дереве типов узлов
    addPage(name, n_page, m_ti_nodeTypes);
    // сообщаем странице процессов, что создан тип узла
    // ему выдадут датчики
    m_p_processes->nodeTypeParamsPageAdded(n_page);
}

void MainWindow::nodeTypeRemoved(NodeTypeParamsPage *n_page, QString name)
{
    // удален тип узла
    // удаляем его страницу
    removePage(n_page);
}

void MainWindow::processAdded(QString name)
{
    // добавлен процесс
    // создаем для него страницу в дереве процессов
    // addPage(name, m_p_new, m_ti_processes);
}

void MainWindow::processRemoved(QString name)
{
    // процесс удален
}

QTreeWidgetItem* MainWindow::addTiWidget(QString name, QTreeWidgetItem* parent)
{
    // добавляем элемент дерева
    QTreeWidgetItem *ti_widget;

    // если указан родитель
    if (parent)
        // то новый элемент дерева будет дочерним от родительского
        ti_widget = new QTreeWidgetItem(parent);
    else
        // иначе новый элемент будет на верхнем уровне дерева
        ti_widget = new QTreeWidgetItem(m_ui->tr_options);

    // выставляем имя элементу
    ti_widget->setText(0, name);
    
    // возвращаем его
    return ti_widget;
}

void MainWindow::addPage(QString m_p_name, QWidget *page, QTreeWidgetItem* parent)
{
    // создаем страницу на объекте страниц
    m_ui->pages->addWidget(page);
    
    // создаем новый элемент дерева для этой страницы
    QTreeWidgetItem *ti_page = addTiWidget(m_p_name, parent);
    
    // запоминаем связь между этими страницами
    m_h_pages.insert(ti_page, page);
}

void MainWindow::addPage(QTreeWidgetItem* ti_page, QWidget *page)
{
    // создаем страницу на объекте страниц
    m_ui->pages->addWidget(page);
    
    // запоминаем связь между переданным элементом дерева и новой страницой
    m_h_pages.insert(ti_page, page);
}

void MainWindow::removePage(QWidget *page)
{
    // удаляем страницу

    // удаляем страницу из объета страниц
    m_ui->pages->removeWidget(page);
    
    // определяем с каким элементом дерева была связана страницы
    QTreeWidgetItem *ti_name = m_h_pages.key(page);
    
    // удаляем элемент дерева
    m_ui->tr_options->removeItemWidget(ti_name, 0);
    
    // удаляем связь
    m_h_pages.remove(ti_name);

    delete page;
    delete ti_name;
}


void MainWindow::saveXml()
{
    // запись данных в xml

    // если не указан путь файла проекта
    if (m_projectFileName == "")
        // ничего не делаем
        return;

    // выделяем память под структуры данных
    dataParams* params = new dataParams();

    // получаем данные

    // параметры симулятора
    params->simulator = getParams();
    // параметры проекта
    params->project = m_p_project->getParams();
    // параметры среды
    params->env = m_p_env->getParams();
    // параметры процесса
    params->processes = m_p_processes->getParams();
    // параметры типов узлов
    params->nodeTypes = m_p_nodeTypes->getParams();
    // параметры узлов
    params->nodes = m_p_nodes->getParams();

    // пишем все в xml
    saveDataParams(m_projectFileName, params);
}

void MainWindow::actionSave()
{
    // нажата кнока сохранить

    // если еще не известно куда сохранять
    if (m_projectFileName == "")
        // спрашиваем полщователя
        actionSaveAs();

    // сохраняем
    saveXml();
    
    m_l_projectName->setText(m_projectFileName);
}

void MainWindow::actionOpen()
{
    // нажата кнопка открыть проект

    // спрашиваем пользователя путь к файлу исходных данных
    m_projectFileName = QFileDialog::getOpenFileName(this,
                                                     tr("Open File"),
                                                     "", "XML Project files (*.xml)");

    if (m_projectFileName != "") {
        // TODO: спрашивать, надо ли их сохранить

        // очищаем созданные данные
        m_p_processes->removeProcesses();
        m_p_nodeTypes->removeNodeTypes();
        
        // загружаем данные
        loadXml();
        
        m_l_projectName->setText(m_projectFileName);
    }
}

void MainWindow::loadXml()
{
    // если указан пустой путь
    if (m_projectFileName == "")
        // то ничего не делаем
        return;

    // получаем данные из xml
    dataParams* params = loadDataParams(m_projectFileName);

    if (params == NULL)
        return;

    // сравниваем версию симулятора с версией данных
    if (params->simulator->version != m_version) {
        // TODO: warning
    }

    // устанавливаем параметры на страницах
    // параметры среды
    m_p_env->setParams(params->env);
    // параметры проекта
    m_p_project->setParams(params->project);
    // параметры процессов
    m_p_processes->setParams(params->processes);
    // паарметры типов узлов
    m_p_nodeTypes->setParams(params->nodeTypes);
    // параметры узлов
    m_p_nodes->setParams(params->nodes);
}

void MainWindow::actionSaveAs()
{
    // нажата кнопка сохранить как

    // спрашиваем пользователя файл
    m_projectFileName = QFileDialog::getSaveFileName(this,
                                                     tr("Save XML Project file"),
                                                     // FIXME: не очень удобно
                                                     "project.xml",
                                                     tr("XML Project file (*.xml)"));

    // сохраняем данные в xml
    saveXml();
}

void MainWindow::actionQuit()
{
    // нажата кнопка выйти

    // TODO: спрашивать сохранить ли результат
    // выходим
    close();
}

simulatorParams* MainWindow::getParams()
{
    // возвращаем параметры симулятора

    // выделяем память для структуры
    simulatorParams* simulator = new simulatorParams();

    if (simulator != NULL) {
        if (simulator->version != NULL) {
            // запсываем в нее версию симулятора
            simulator->version = m_version;
        }
    }

    // возвращаем ее
    return simulator;
}
