#ifndef ENVPAGE_H
#define ENVPAGE_H

#include "dataParams.h"
#include <QGroupBox>

namespace Ui {
    class EnvPage;
}

class EnvPage : public QGroupBox
{
    Q_OBJECT

public:
    EnvPage();
    virtual ~EnvPage();

    // запись параметров среды
    void setParams(envParams* params);
    // чтение параметров среды
    envParams* getParams();
    
signals:
    // сигнал об изменении размеров среды
    // первый агрумент - новое значение размера среды
    // второй аргумент - координата, значеине которой изменилось. 0 - x, 1 - y, 2 - z
    void envSizeChanged(double value, int coord);

private slots:
    // слоты, обрабатывающие изменения размеров среды
    void sizeXChanged(double value);
    void sizeYChanged(double value);
    void sizeZChanged(double value);

    // void openEnvMovementFile();
    // void editEnvMovementFile();

private:
    
    Ui::EnvPage *m_ui;
};

#endif // ENVPAGE_H
