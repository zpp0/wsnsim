#include <QFileDialog>
#include <QTextCodec>
#include <QProcess>
#include <QMessageBox>

#include "mainWindow.h"
#include "ui_main.h"

MainWindow::MainWindow() :
    QMainWindow(),
    m_ui(new Ui::MainWindow)
{
    // инициализируем интерфейс
    m_ui->setupUi(this);

    qDebug() << "started";
    
    m_core = NULL;
    qDebug() << "creating new thread";
    m_thread = new MainWindowThread(this);
    m_warning = false;
    
    // устанавливаем кодировки для строк
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

    qDebug() << "disable warnings";
    disableWarnings();
    
    // m_ui->stacked->hide();
    
    // нажата кнопка добавления процесса
    connect(m_ui->tb_openProject, SIGNAL(clicked()),
            this, SLOT(actionOpen()));

    connect(m_ui->actionQuit, SIGNAL(triggered()),
            this, SLOT(actionQuit()));

    connect(this, SIGNAL(openedNewProject()),
            this, SLOT(enableButtons()));

    connect(m_ui->b_start, SIGNAL(clicked()),
            this, SLOT(actionStart()));

    // connect(this, SIGNAL(stopping()),
    //         m_thread, SLOT(stop()));

}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    // while(m_thread->isRunning());
    // if (m_thread->isRunning()) {
        // m_thread->terminate();
    qDebug() << "closing...";
    qDebug() << "deleting thread";
    delete m_thread;
    // }
    if (m_core) {
        qDebug() << "deleting core";
        // m_core->kill();
        delete m_core;
    }
}

void MainWindow::actionQuit()
{
    qDebug() << "actionQuit";
    
    if (!m_core) {
        qDebug() << "quit";
        close();
        return;
    }

    qDebug() << "core started, print messagebox";
    QMessageBox msgBox;
    msgBox.setText("The simulator has been started.");
    msgBox.setInformativeText("Do you want to stop it?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok) {
        qDebug() << "quit...";
        qDebug() << "emit stopping";
        emit stopping();
        qDebug() << "close";
        close();
    }
}

void MainWindow::enableWarnings()
{
    qDebug() << "enable warnings";
    m_ui->stacked->setCurrentIndex(0);
    // m_ui->te_warnings->show();
    // m_ui->centralwidget->adjustSize();
}

void MainWindow::disableWarnings()
{
    qDebug() << "disable warnings";
    m_ui->stacked->setCurrentIndex(1);
    // m_ui->stacked->hide();
    // m_ui->centralwidget->adjustSize();
}

void MainWindow::actionOpen()
{
    // нажата кнопка открыть проект

    qDebug() << "open project";
    
    // спрашиваем пользователя путь к файлу исходных данных
    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                "", "XML Project files (*.xml)");
    
    if (file != "") {
        qDebug() << "print path";
        m_ui->le_fileProject->setText(file);
        qDebug() << "enable buttons";
        enableButtons();
        qDebug() << "disable warnings";
        // emit openedNewProject();
        disableWarnings();
    }
}

void MainWindow::enableButtons()
{
    m_ui->b_edit->setEnabled(true);
    m_ui->b_result->setEnabled(true);
    m_ui->b_start->setEnabled(true);
    m_ui->cb_format->setEnabled(true);
    m_ui->cb_debugEnable->setEnabled(true);
    m_ui->tb_openProject->setEnabled(true);
    // disableWarnings();
}

void MainWindow::actionStart()
{
    disableWarnings();
    qDebug() << "clear warnings text";
    m_ui->te_warnings->clear();
    qDebug() << "preparing thread";
    prepareThread(m_thread);
    qDebug() << "start thread";

    disconnect(m_ui->b_start, SIGNAL(clicked()),
               this, SLOT(actionStart()));

    connect(m_ui->b_start, SIGNAL(clicked()),
            this, SLOT(stopCore()));

    m_thread->start();
}

void MainWindow::startCore()
{
    qDebug() << "starting core";
    
    if (!m_core) {
        qDebug() << "creating new core";
        m_core = new QProcess();
    }
    
    QStringList arguments;
    arguments << m_ui->le_fileProject->text() << m_ui->cb_format->currentText();
    qDebug() << "starting";
    m_core->start("./core", arguments);
    
    connect(m_core, SIGNAL(readyReadStandardOutput()),
            this, SLOT(updateProgressBar()));
    connect(m_core, SIGNAL(readyReadStandardError()),
            this, SLOT(updateWarningText()));

    qDebug() << "waiting for finished";
    // m_core->waitForFinished(-1);
    m_core->waitForFinished();
}

void MainWindow::stopCore()
{
    qDebug() << "stopping core";
    qDebug() << "core->kill()";
    // qDebug() << "core state is" << m_core->state();
    m_core->kill();
    // m_core->waitForFinished(-1);
    m_core->waitForFinished();
}

void MainWindow::updateProgressBar()
{
    qDebug() << "updating progressbar";
    // читаем число из stdout и устанавливаем его как значение прогресс бара
    m_ui->progressBar->setValue(m_core->readAllStandardOutput().toInt());
}

void MainWindow::updateWarningText()
{
    if (m_ui->stacked->currentIndex() == 1)
        enableWarnings();

    qDebug() << "append new warning";
    m_ui->te_warnings->append(QString::fromLocal8Bit(m_core->readAllStandardError()));

    m_warning = true;
    // QString warningEntry = QString::fromLocal8Bit(m_core->readAllStandardError());

    // // получаем текущее количество узлов
    // int row = m_ui->t_warnings->rowCount();

    // m_ui->t_warnings->insertRow(row);

    // QTableWidgetItem* iconItem;
    // QLabel* text = new QLabel(warningEntry);
    
    // if (warningEntry.contains("Warning:", Qt::CaseSensitive)) {
    //     iconItem = new QTableWidgetItem(QIcon(":/res/dialog-ok-apply.png"), "");
    // }
    // else
    //     iconItem = new QTableWidgetItem(QIcon(":/res/dialog-ok.png"), "");
    
    // m_ui->t_warnings->setItem(row, 0, iconItem);
    // m_ui->t_warnings->setCellWidget(row, 1, text);
    
    // // m_ui->t_warnings->append();
}

void MainWindow::disableButtons()
{
    m_ui->b_edit->setEnabled(false);
    m_ui->tb_openProject->setEnabled(false);
    m_ui->cb_format->setEnabled(false);
    m_ui->cb_debugEnable->setEnabled(false);
    // m_ui->b_result->setEnabled(false);
    // m_ui->b_start->setEnabled(false);
    // disableWarnings();
}

void MainWindow::prepareThread(MainWindowThread *&thread)
{
    connect(thread, SIGNAL(started()),
            this, SLOT(thisThreadStarted()), Qt::BlockingQueuedConnection);
    connect(thread, SIGNAL(finished()),
            this, SLOT(thisThreadFinished()), Qt::BlockingQueuedConnection);
    connect(thread, SIGNAL(terminated()),
            this, SLOT(thisThreadFinished()), Qt::BlockingQueuedConnection);
}

void MainWindow::thisThreadStarted()
{
    
    // connect(m_ui->actionQuit, SIGNAL(clicked()),
    //         this, SLOT(stopCore()));

    QIcon stopIcon(":/res/process-stop.png");
    m_ui->b_start->setIcon(stopIcon);
    m_ui->b_start->setText("Stop");

    QIcon clockIcon(":/res/view-history.png");
    m_ui->b_result->setIcon(clockIcon);
    
    disableButtons();
}

void MainWindow::thisThreadFinished()
{
    disconnect(m_ui->b_start, SIGNAL(clicked()),
               this, SLOT(stopCore()));

    connect(m_ui->b_start, SIGNAL(clicked()),
            this, SLOT(actionStart()));

    delete m_core;
    m_core = NULL;
    
    QIcon stopIcon(":/res/arrow-right");
    m_ui->b_start->setIcon(stopIcon);
    m_ui->b_start->setText("Start");

    if ((m_ui->progressBar->value() == 100) && (m_warning == false))
        m_ui->b_result->setIcon(QIcon(":/res/dialog-ok-apply.png"));
    else
        m_ui->b_result->setIcon(QIcon(":/res/dialog-ok.png"));

    enableButtons();

    m_warning = true;
}

// void MainWindow::thisThreadTerminated()
// {
//     QIcon stopIcon(":/res/arrow-right");
//     m_ui->b_start->setIcon(stopIcon);
//     m_ui->b_start->setText("Start");

//     QIcon clockIcon(":/res/dialog-ok.png");
//     m_ui->b_result->setIcon(clockIcon);

//     enableButtons();

//     disconnect(m_ui->b_start, SIGNAL(clicked()),
//                this, SLOT(stopCore()));

//     connect(m_ui->b_start, SIGNAL(clicked()),
//             this, SLOT(actionStart()));
    
// }
