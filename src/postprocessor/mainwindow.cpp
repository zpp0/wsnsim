#include <QtGui>
#include "filter.h"
#include "index.h"
#include "tableview.h"
#include "mainwindow.h"
#include "settingdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //читаем пользовательские настройки
    //в зависимости от сохраненных настроек устанавливаем нужное представление данных
    //

    //задаем фильтр событий
    m_pfilter = new Filter;
    //индекс еще не создан
    m_pindex = NULL;

    //создаем виджет отображения данных симуляции и устанавливаем его как центральный виджет

    //создаем меню и действия
    createActions();
    createMenus();

    //устанавливаем иконку приложения
    //setWindowIcon(QIcon(":/images/icon.png"));

}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions()
{
    //создаем и настраиваем действие открыть файл журнала
    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open a log file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    //создаем и настраиваем дейстивие выход
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //создаем и настраиваем действие изменить фильтр отображаемых событий
    m_psettingViewAction = new QAction(tr("О&тображаемые события"), this);
    m_psettingViewAction->setShortcut(tr("Ctrl+t"));
    m_psettingViewAction->setStatusTip(tr("Какие события отображать"));
    m_psettingViewAction->setCheckable(true);
    connect(m_psettingViewAction, SIGNAL(triggered()), this, SLOT(setting()));

    //создаем группу дейстий по переключению представления данных
    m_pViewGroup = new QActionGroup(this);

//    m_pViewGroup->addAction(m_psettingViewAction);
//    m_pViewGroup->addAction(openAction);
//    m_psettingViewAction->setChecked(true);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Вид"));
    //editMenu->addAction();
    //editMenu->addActions();
    editMenu->addActions(m_pViewGroup->actions());

    settingsMenu = menuBar()->addMenu(tr("&Настройки"));
    settingsMenu->addAction(m_psettingViewAction);

    //optionsMenu = menuBar()->addMenu(tr("&Options"));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Помощь"));

}

void MainWindow::open()
{
    QString sourseFileName = QFileDialog::getOpenFileName(this, tr("Open log file"), ".", tr("xml files (*.xml)"));


    //если пользователь выбрал файл исходных данных
    if (!sourseFileName.isEmpty())
    {
        //преобразуем исходную строку с именем xml-файла чтобы получить имя файла журнала
        QString logFileName = sourseFileName;
        logFileName.replace(logFileName.length() - 4, logFileName.length() - 4, ".log");


        //считываем настройки фильтра событий, если таковые уже производились
        //(если существует файл ./settings/filter.st)
        QString settingFileName = sourseFileName;
        settingFileName.replace(QRegExp("[a-zA-Z_0-9]*.xml"), "settings/setting.st");
        //открываем файл настройки
        //QFile settings(settingFileName);
        //запоминаем в фильтре где хранятся/будут храниться его настройки
        m_pfilter->setSettingFile(settingFileName);

        //если индекс уже был ранее сконструирован
        if ( m_pindex )
        {
            //удаляем его
            delete m_pindex;
        }
        //создаем индекс для этого файла
        m_pindex = new Index(logFileName, m_pfilter);

        //создаем нужное представление (зависит от настроек), NOTE: пока только текстовое
        m_pTableView = new TableView(m_pindex);
        setCentralWidget(m_pTableView);

        //загружаем его
//        if ( !m_pDataViewer->loadData(sourseFileName) )
//        {
//            statusBar()->showMessage(tr("Loading canceled"), 2000);
//        }
    }
}

void MainWindow::setting()
{
    //создаем модальное окно настройки отображаемых событий
    SettingDialog * dialog = new SettingDialog(m_pfilter, this);
    //соединяем сигнал изменения настроек в диалоговом окне со слотом измения фильтра
    connect(dialog, SIGNAL(signalFilterChanged(QVector<quint8>)), m_pfilter, SLOT(setFilter(QVector<quint8>)));
    //делаем окно модальным
    dialog->exec();
    delete dialog;
}
