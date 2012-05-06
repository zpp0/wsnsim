#include <QFileDialog>
#include <cstdlib>

#include "dataParams.h"

#include "envPage.h"
#include "ui_envPage.h"

EnvPage::EnvPage () :
    m_ui(new Ui::EnvPage)
{
    // инициализируем интерфейс
    m_ui->setupUi(this);

    // сигналы

    // нажатие spin box'ов обрабатывает страница среды
    connect(m_ui->dsb_xSize, SIGNAL(valueChanged(double)),
            this, SLOT(sizeXChanged(double)));

    connect(m_ui->dsb_ySize, SIGNAL(valueChanged(double)),
            this, SLOT(sizeYChanged(double)));

    connect(m_ui->dsb_zSize, SIGNAL(valueChanged(double)),
            this, SLOT(sizeZChanged(double)));

    // // сигнал нажатия кнопки открытия файла
    // connect(m_ui->tb_envMovement, SIGNAL(clicked()),
    //         this, SLOT(openEnvMovementFile()));
    
    // connect(m_ui->tb_envMovementEdit, SIGNAL(clicked()),
    //         this, SLOT(editEnvMovementFile()));
    
}

EnvPage::~EnvPage ()
{
    delete m_ui;
}

void EnvPage::sizeXChanged (double value)
{
    emit envSizeChanged (value, 0);
}

void EnvPage::sizeYChanged (double value)
{
    emit envSizeChanged (value, 1);
}

void EnvPage::sizeZChanged (double value)
{
    emit envSizeChanged (value, 2);
}

void EnvPage::setParams(envParams* params)
{
    if (params != NULL) {
        if (params->size != NULL) {
            // устанвливаем размеры среды
            m_ui->dsb_xSize->setValue(params->size[0]);
            m_ui->dsb_ySize->setValue(params->size[1]);
            m_ui->dsb_zSize->setValue(params->size[2]);
        }
        if (params->maxSimulatorWorkTime != NULL)
            // устанавливаем время
            m_ui->sb_maxTime->setValue(params->maxSimulatorWorkTime);
        if (params->timeUnits != NULL) 
            m_ui->cb_timeUnits->setCurrentIndex(params->timeUnits);
    }
}

envParams* EnvPage::getParams()
{
    envParams* params = new envParams();

    // записываем размеры
    params->size[0] = m_ui->dsb_xSize->value();
    params->size[1] = m_ui->dsb_ySize->value();
    params->size[2] = m_ui->dsb_zSize->value();

    // записываем время работы
    params->maxSimulatorWorkTime = m_ui->sb_maxTime->value();
    params->timeUnits = simulatorTimeUnits(m_ui->cb_timeUnits->currentIndex());
    
    // возвращаем
    return params;
}
