#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QProcess>
#include <QDebug>

namespace Ui {
    class MainWindow;
}

class MainWindowThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();
        
private:
    QProcess* m_core;
    MainWindowThread* m_thread;

    Ui::MainWindow* m_ui;

    bool m_warning;
    
    void enableWarnings();
    void disableWarnings();

    void setStartButtonIcon();
    void setResultButtonIcon();

    void prepareThread(MainWindowThread *&thread);
    
signals:
    void openedNewProject();
    void stopping();

protected:
  void closeEvent(QCloseEvent *event);
    
private slots:
    // события
    // выйти
    void actionQuit();
    void actionOpen();
    void actionStart();
    // void actionEdit();
    // void actionResult();
    
    // void heavyDuty();
    void thisThreadStarted();
    void thisThreadFinished();
    // void thisThreadTerminated();
    // void setProgress(int pos);

    void updateProgressBar();
    void updateWarningText();
    void enableButtons();
    void disableButtons();

public slots:

    void startCore();
    void stopCore();

};

class MainWindowThread : public QThread
{
    Q_OBJECT

public:
    MainWindowThread(MainWindow *mainWindow) : mainWindow(mainWindow){};
    virtual ~MainWindowThread(){};
    
    void run() { mainWindow->startCore(); }
    // void terminate() { mainWindow->stopCore(); }

public slots:
    void stop() { mainWindow->stopCore(); }

protected:
    MainWindow *mainWindow;
};

#endif // MAINWINDOW_H
