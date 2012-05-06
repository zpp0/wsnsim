#ifndef NODETYPEPARAMSPAGE_H
#define NODETYPEPARAMSPAGE_H

#include <QGroupBox>
#include <QLineEdit>
#include <QString>
#include <QVector>

// #include "processParamsPage.h"

#include "xml.h"
#include "dataParams.h"

namespace Ui {
    class NodeTypeParamsPage;
}

class NodeTypeParamsPage : public QGroupBox
{
    Q_OBJECT

public:
    NodeTypeParamsPage(QString nt_name);
    virtual ~NodeTypeParamsPage();

    // запись параметров типов узлов
    void setParams(nodeTypeParams* params);
    // чтение параметров типов узлов
    nodeTypeParams* getParams();
    
public slots:
    // слот для обработки события процесс добавлен
    void processAdded(QString name);
    // слот для обработки события процесс удален
    void processRemoved(QString name);
    // слот для обработки события единицы измерений процесса изменены
    void processUnitChanged(quint8 processID, QString units);
    void processNameChanged(quint8 processID, QString name);
                                                                             
private slots:
    // обработка событий выбора файлов
    void chooseLuaProgrammFile();

    // обработка событий редактирования файлов
    void openLuaProgrammFile();

    // обработка событий выбора файлов
    void chooseLuaStartTimeFile();

    // обработка событий редактирования файлов
    void openLuaStartTimeFile();
    
    // обработка события нажата кнопка добавления 
    void createTimerDependence();
    void deleteTimerDependence();

    void moveDownTimerDependence();
    void moveUpTimerDependence();

    void toggleDirectionAntennaFunction();
    void toggleRealTimerFunction();
    void toggleMovementFunction();
    
private:

    void addFunctionLabel(QString label, int tableIndex = 0);
    void toggleFunctionLabel(QString label, int tableIndex = 0);
    
    void addSensor(QString name);
    void removeSensor(QString name);
    
    void addTimerDependence(QString name);
    void removeTimerDependence(QString name);
    
    Ui::NodeTypeParamsPage *m_ui;

    // имя типа узла
    QString m_name;
    
    quint16 m_ID;

    // определение номера строки по имени датчика в таблице датчиков
    int pt_findRow(QString pt_name);
};

#endif // NODETYPEPARAMSPAGE_H
