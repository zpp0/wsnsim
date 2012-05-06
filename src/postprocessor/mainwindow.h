#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>



class QAction;
class QActionGroup;
class Index;
class Filter;
class LogViewer;
class TableView;

class DataViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    //void closeEvent(QCloseEvent *event);

private slots:
    void open();
    void setting();

private:
    void createActions();
    void createMenus();
//    void createContextMenu();
//    void createToolBars();
//    void createStatusBar();
//    void readSettings();
//    void writeSettings();
//    bool okToContinue();
//    //void setCurrentFile(const QString  &fullFileName);

    //
    DataViewer *m_pDataViewer;

    //индекс файла журнала
    Index * m_pindex;
    //фильтр событий LogEvent
    Filter * m_pfilter;
    //представления данных
    //текстовое представление
    TableView * m_pTableView;
    //...


    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *settingsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;

    QAction *openAction;
    QAction *exitAction;

    QActionGroup *m_pViewGroup;

    QAction *m_psettingViewAction;
};

#endif // MAINWINDOW_H
