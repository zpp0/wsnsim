/***********************************************
 *
 * File: xml.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include "xml.h"

#include <QtXml>
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>

#include <QVector>
#include <QDebug>

void createXml(QDomDocument* result, QDomElement* parent, QString XNodeName, QString XNodeValue)
{
    QDomElement de_new = result->createElement(XNodeName);
    QDomText dt_new = result->createTextNode(XNodeValue);
    de_new.appendChild(dt_new);
    parent->appendChild(de_new);
}

void createXml(QDomDocument* result, QDomElement* parent, QString XNodeName, double XNodeValue)
{
    createXml(result, parent, XNodeName, QString::number(XNodeValue));
}

void saveEnvParams(QDomDocument* result, QDomElement* parent, envParams* params)
{
    // создаем узел Env, дочерними которого будут узлы-параметры среды
    QDomElement de_envSize = result->createElement("Env");

    // создаем узлы с размерами среды
    createXml(result, &de_envSize, "X", params->size[0]);
    createXml(result, &de_envSize, "Y", params->size[1]);
    createXml(result, &de_envSize, "Z", params->size[2]);
    // создаем узел с максимальным временем работы симулятора
    createXml(result, &de_envSize, "maxTime", params->maxSimulatorWorkTime);
    // создаем узел с единицами измерений времени работы симулятора
    createXml(result, &de_envSize, "timeUnits", params->timeUnits);

    // прикрепляем поддерево xml с корнем Env к общему дереву
    parent->appendChild(de_envSize);
}

envParams* loadEnvParams(QDomNode dn_node)
{
    // выделяем память под структуру
    envParams* params = new envParams();
    
    // получаем первый дочерний узел поддерева параметров среды
    QDomNode dn_nextNode = dn_node.firstChild();

    // пока есть узлы
    while (!dn_nextNode.isNull()) {

        // узел - размер среды по X
        if (dn_nextNode.nodeName() == "X")
            params->size[0] = dn_nextNode.toElement().text().toDouble();
        // узел - размер среды по Y
        if (dn_nextNode.nodeName() == "Y")
            params->size[1] = dn_nextNode.toElement().text().toDouble();
        // узел - размер среды по Z
        if (dn_nextNode.nodeName() == "Z")
            params->size[2] = dn_nextNode.toElement().text().toDouble();
        // узел - максимальное время работы симулятора
        if (dn_nextNode.nodeName() == "maxTime")
            params->maxSimulatorWorkTime = dn_nextNode.toElement().text().toDouble();
        // узел - единицы измерений времени работы симулятора
        if (dn_nextNode.nodeName() == "timeUnits")
            params->timeUnits = simulatorTimeUnits(dn_nextNode.toElement().text().toInt());

        // переходим к следующему узлу
        dn_nextNode = dn_nextNode.nextSibling();
    }

// возвращаем указатель на структуру
    return params;
}

void saveNodeParams(QDomDocument* result, QDomElement* parent, nodeParams* params)
{
        // создаем XML узлы с описанием характеристик типа узла БСС
        // как дочерние к главному узлу

        // создаем XML узел node для узла БСС
        QDomElement de_nodeItem = result->createElement("node");

        // создаем XML узел nodeID со значением типа узла
        createXml(result, &de_nodeItem, "nodeID", params->nodeID);
        // создаем XML узел nodeTypeID с номером типа узла
        createXml(result, &de_nodeItem, "nodeTypeID", params->nodeTypeID);

        // создаем XML узел X со значением кординаты узла по оси x
        createXml(result, &de_nodeItem, "X", params->coord[0]);

        // создаем XML узел Y со значением кординаты узла по оси y
        createXml(result, &de_nodeItem, "Y", params->coord[1]);

        // создаем XML узел Z со значением кординаты узла по оси z
        createXml(result, &de_nodeItem, "Z", params->coord[2]);

        // создаем XML узел random со значением 1 или 0
        createXml(result, &de_nodeItem, "random", params->random);

        // добавляем в качестве дочернего узла node к главному
        parent->appendChild(de_nodeItem);
}

nodeParams* loadNodeParams(QDomNode dn_node)
{
    // выделяем память для структуры
    nodeParams* params = new nodeParams();
    // получаем первый дочерний узел поддерева
    QDomNode dn_nextParam = dn_node.firstChild();

    // распознаем узлы, пока они не закончатся
    while (!dn_nextParam.isNull()) {

        // узел - nodeID
        if (dn_nextParam.nodeName() == "nodeID")
            params->nodeID = dn_nextParam.toElement().text().toInt();
        // узел - nodeTypeID
        if (dn_nextParam.nodeName() == "nodeTypeID")
            params->nodeTypeID = dn_nextParam.toElement().text().toInt();
        // узел - координата узла по X
        if (dn_nextParam.nodeName() == "X")
            params->coord[0] = dn_nextParam.toElement().text().toDouble();
        // узел - координата узла по Y
        if (dn_nextParam.nodeName() == "Y")
            params->coord[1] = dn_nextParam.toElement().text().toDouble();
        // узел - координата узла по Z
        if (dn_nextParam.nodeName() == "Z")
            params->coord[2] = dn_nextParam.toElement().text().toDouble();
        // узел - случайно ли расположен узел?
        if (dn_nextParam.nodeName() == "random")
            params->random = dn_nextParam.toElement().text().toInt();
        
        // переходим к следующему узлу
        dn_nextParam = dn_nextParam.nextSibling();
    }

    // возвращаем результат
    return params;
}

QVector<nodeParams*> loadNodesParams(QDomNode dn_node)
{
    // выделяем память пол массив указателей на структуры
    QVector<nodeParams*> params;

    // получаем первый дочерний узел поддерева
    QDomNode dn_nextNode = dn_node.firstChild();

    // распознаем узлы, пока они не закончатся
    while (!dn_nextNode.isNull()) {

        // узел - node
        if (dn_nextNode.nodeName() == "node")
            params.append(loadNodeParams(dn_nextNode));

        // переходим к следующему узлу
        dn_nextNode = dn_nextNode.nextSibling();
    }

    // возвращаем результат
    return params;
}

void saveNodesParams(QDomDocument* result, QDomElement* parent, QVector <nodeParams*> params)
{

    // создаем элемент дерева, дочерними которого будут узлы с их параметрами
    QDomElement de_nodeslocation = result->createElement("NodesList");

    // для всех элементов массива
    for (int i = 0; i < params.size(); i++)
        // записываем параметы узлов
        saveNodeParams(result, &de_nodeslocation, params[i]);
    
    // добавляем главный XML узел к обшему дереву
    parent->appendChild(de_nodeslocation);
}

sensorParams* loadSensorParams(QDomNode dn_node)
{
    // выделяем память для структуры
    sensorParams* params = new sensorParams();

    // получаем первый дочерний узел поддерева
    QDomNode dn_sensor = dn_node.firstChild();

    // распознаем узлы, пока они не закончатся
    while (!dn_sensor.isNull()) {

        // узел - name
        if (dn_sensor.nodeName() == "name")
            params->name = dn_sensor.toElement().text();
        // узел - sensorID
        if (dn_sensor.nodeName() == "sensorID")
            params->sensorID = dn_sensor.toElement().text().toInt();
        // узел - processID
        if (dn_sensor.nodeName() == "processID")
            params->processID = dn_sensor.toElement().text().toInt();
        // узел - units
        if (dn_sensor.nodeName() == "units")
            params->units = dn_sensor.toElement().text();
        // узел - minValue
        if (dn_sensor.nodeName() == "minValue")
            params->minValue = dn_sensor.toElement().text().toDouble();
        // узел - maxValue
        if (dn_sensor.nodeName() == "maxValue")
            params->maxValue = dn_sensor.toElement().text().toDouble();

        // переходим к следующему узлу
        dn_sensor = dn_sensor.nextSibling();
    }

    // возвращаем результат
    return params;
}

void saveSensorParams(QDomDocument* result, QDomElement* parent, sensorParams* params)
{
    // создаем узел sensor, дочерними которого будут узлы-параметры этого датчика
    QDomElement de_sensor = result->createElement("sensor");
    
    // пишем ID датчика
    createXml(result, &de_sensor, "sensorID", params->sensorID);
    // пишем ID измеряемого процесса
    createXml(result, &de_sensor, "processID", params->processID);
    // пишем имя датчика
    createXml(result, &de_sensor, "name", params->name);
    // пишем единицы измерений
    createXml(result, &de_sensor, "units", params->units);
    // пишем нижную границу измеряемого диапазона
    createXml(result, &de_sensor, "minValue", params->minValue);
    // пишем верхнюю границу измеряемого диапазона
    createXml(result, &de_sensor, "maxValue", params->maxValue);

    // прикрепяем узел sensor с его параметрами к родительскому элементу - собираем дерево
    parent->appendChild(de_sensor);
}

void saveNodeTypeParams(QDomDocument* result, QDomElement* parent, nodeTypeParams* params)
{
    // создаем элемент дерева NodeType, дочерними которого будут элементы с параметрами типа узла
    QDomElement de_ntp = result->createElement("NodeType");

    // создаем узел nodeTypeID
    createXml(result, &de_ntp, "nodeTypeID", params->nodeTypeID);
    // создаем узел name
    createXml(result, &de_ntp, "name", params->name);

    // создаем узел TransmitterPower
    createXml(result, &de_ntp, "TransmitterPower", params->transmitterPower);
    // создаем узел ReceiverSensivity
    createXml(result, &de_ntp, "ReceiverSensivity", params->receiverSensivity);
    // создаем узел freqBand
    createXml(result, &de_ntp, "freqBand", params->frequency);
    // создаем узел directionalAntenna
    createXml(result, &de_ntp, "directionalAntenna", params->directionalAntenna);

    // создем узел Unrenewable
    createXml(result, &de_ntp, "Unrenewable", params->unrenewableEnergy);
    // создаем узел Energy
    createXml(result, &de_ntp, "Energy", params->energy);

    // создаем узел SensorsList, дочерними которого будут датчики и их параметры
    QDomElement de_sensors = result->createElement("SensorsList");

    // для каждого датчика из массива
    for (int i = 0; i < params->sensors.size(); i++)
        // записываем параметры датчика
        saveSensorParams(result, &de_sensors, params->sensors[i]);

    // прикрепляем дерево датчиков к параметрам типа узла
    de_ntp.appendChild(de_sensors);

    // создаем узел Movement
    createXml(result, &de_ntp, "Movement", params->movement);
    // создаем узел RealClock
    createXml(result, &de_ntp, "RealClock", params->realClock);

    // записываем список ID процессов, от которых зависят часы
    QDomElement de_clockDependensies = result->createElement("RealClockProcessDependenciesList");

    // заполняем список
    for (int i = 0; i < params->clockProcessDependence.size(); i++)
        createXml(result, &de_clockDependensies, "dependProcessID", params->clockProcessDependence[i]);

    // прикрепляем список к общему дереву
    de_ntp.appendChild(de_clockDependensies);
    
    // создаем узлы с файлами к программам
    createXml(result, &de_ntp, "luaFile", params->luaFile);

    // создаем поддерево с функциями программы
    QDomElement de_functions = result->createElement("Functions");

    // описания функций
    QList<QString> funcLabels = params->functions.keys();
    // для каждого названия
    for (int i = 0; i < funcLabels.size(); i++) {
        // создаем поддерево с названием Function
        QDomElement de_function = result->createElement("Function");
        // элементы которого
        // funcLabel - описание функции
        createXml(result, &de_function, "funcLabel", funcLabels[i]);
        // и funcName - названия функции
        createXml(result, &de_function, "funcName", params->functions[funcLabels[i]]);
        // прикрепляем поддерево к дереву Functions
        de_functions.appendChild(de_function);
    }

    // прикрепляем поддерево Functions к дереву параметров типа узла
    de_ntp.appendChild(de_functions);
    
    // прикрепляем дерево с параметрами типа узла к общему дереву
    parent->appendChild(de_ntp);
}

nodeTypeParams* loadNodeTypeParams(QDomNode dn_node)
{
    // выделяем память для структуры
    nodeTypeParams* params = new nodeTypeParams();
    // создаем массив датчиков
    QVector<sensorParams*> sensors;
    // прикрепляем массив датчиков к параметрам типа узла
    params->sensors = sensors;    

    // получаем первый дочерний узел поддерева
    QDomNode dn_nextNode = dn_node.firstChild();

    // распознаем узлы, пока они не закончатся
    while (!dn_nextNode.isNull()) {

        // узел - nodeTypeID
        if (dn_nextNode.nodeName() == "nodeTypeID")
            params->nodeTypeID = dn_nextNode.toElement().text().toInt();
        // узел - node
        if (dn_nextNode.nodeName() == "name")
            params->name = dn_nextNode.toElement().text();

        // узел - TransmitterPower
        if (dn_nextNode.nodeName() == "TransmitterPower")
            params->transmitterPower = dn_nextNode.toElement().text().toDouble();
        // узел - ReceiverSensivity
        if (dn_nextNode.nodeName() == "ReceiverSensivity")
            params->receiverSensivity = dn_nextNode.toElement().text().toDouble();
        // узел - freqBand
        if (dn_nextNode.nodeName() == "freqBand")
            params->frequency = freqBand(dn_nextNode.toElement().text().toInt());
        // узел - directionalAntenna
        if (dn_nextNode.nodeName() == "directionalAntenna")
            params->directionalAntenna = freqBand(dn_nextNode.toElement().text().toInt());

        // узел - Unrenewable
        if (dn_nextNode.nodeName() == "Unrenewable")
            params->unrenewableEnergy = dn_nextNode.toElement().text().toInt();
        // узел - Energy
        if (dn_nextNode.nodeName() == "Energy")
            params->energy = dn_nextNode.toElement().text().toInt();

        // узел - Movement
        if (dn_nextNode.nodeName() == "Movement")
            params->movement = dn_nextNode.toElement().text().toInt();

        // узел - RealClock
        if (dn_nextNode.nodeName() == "RealClock")
            params->realClock = dn_nextNode.toElement().text().toInt();

        // узел RealClockProcessDependenciesList
        if (dn_nextNode.nodeName() == "RealClockProcessDependenciesList") {
            // у него есть дочерние узлы
            QDomNode dn_paramsNode = dn_nextNode.firstChild();
            
            // распознаем дочерние узлы, пока они не закончатся
            while (!dn_paramsNode.isNull()) {
                
                // узел - dependProcessID
                if (dn_paramsNode.nodeName() == "dependProcessID")
                    // распознаем получаем ID
                    params->clockProcessDependence.append(dn_paramsNode.toElement().text().toInt());

                // перходим к следующему узлу списка ID процессов
                dn_paramsNode = dn_paramsNode.nextSibling();
            }
        }
        
        // узел - SensorsList
        if (dn_nextNode.nodeName() == "SensorsList") {
            // у него есть дочерние узлы
            QDomNode dn_paramsNode = dn_nextNode.firstChild();

            // распознаем дочерние узлы, пока они не закончатся
            while (!dn_paramsNode.isNull()) {

                // узел - sensor
                if (dn_paramsNode.nodeName() == "sensor")
                    // распознаем параметры датчика
                    params->sensors.append(loadSensorParams(dn_paramsNode));

                // перходим к следующему узлу списка датчиков
                dn_paramsNode = dn_paramsNode.nextSibling();
            }
        }

        // узел - luaFile
        if (dn_nextNode.nodeName() == "luaFile")
            params->luaFile = dn_nextNode.toElement().text();
        
        if (dn_nextNode.nodeName() == "Functions") {
            // у него есть дочерние узлы
            QDomNode dn_paramsNode = dn_nextNode.firstChild();
            
            // распознаем дочерние узлы, пока они не закончатся
            while (!dn_paramsNode.isNull()) {
                if (dn_paramsNode.nodeName() == "Function") {
                    // у него есть дочерние узлы
                    QDomNode dn_funcNode = dn_paramsNode.firstChild();

                    QString funcLabel = "";

                    while (!dn_funcNode.isNull()) {
                        if (dn_funcNode.nodeName() == "funcLabel")
                            funcLabel = dn_funcNode.toElement().text();
                        if (dn_funcNode.nodeName() == "funcName")
                            if (funcLabel != "")
                                params->functions[funcLabel] = dn_funcNode.toElement().text();
                        
                        dn_funcNode = dn_funcNode.nextSibling();
                    }
                }

                dn_paramsNode = dn_paramsNode.nextSibling();
            }
        }

        // переходим к следующему узлу
        dn_nextNode = dn_nextNode.nextSibling();
    }

    // возвращаем результат
    return params;
}

void saveNodeTypesParams(QDomDocument* result, QDomElement* parent, QVector<nodeTypeParams*> params)
{
    // создаем узел NodeTypesList, дочерними которго будут параметры типов узлов
    QDomElement de_nodetypes = result->createElement("NodeTypesList");

    // для всех элементов массива типов узлов
    for (int i = 0; i < params.size(); i++)
        // записываем параметры типа узлов
        saveNodeTypeParams(result, &de_nodetypes, params[i]);

    // прикрепляем дерево параметров типов узлов к общему дереву
    parent->appendChild(de_nodetypes);
}

QVector<nodeTypeParams*> loadNodeTypesParams(QDomNode dn_node)
{
    // создаем массив параметров типов узлов
    QVector<nodeTypeParams*> params;

    QDomNode dn_nextNode = dn_node.firstChild();
    
    // перебираем узлы, пока не кончатся
    while (!dn_nextNode.isNull()) {

        // узел - NodeType
        if (dn_nextNode.nodeName() == "NodeType")
            // анализируем параметры типа узла
            params.append(loadNodeTypeParams(dn_nextNode));

        // переходим к следующему узлу
        dn_nextNode = dn_nextNode.nextSibling();
    }

    // возвращаем результат
    return params;
}

void saveProcessParams(QDomDocument* result, QDomElement* parent, processParams* params)
{
    // создаем узел Process, дочерними которого являются параметры этого процесса
    QDomElement de_ptp = result->createElement("Process");

    // создаем узел processID
    createXml(result, &de_ptp, "processID", params->processID);
    // создаем узел name
    createXml(result, &de_ptp, "name", params->name);
    // создаем узел units
    createXml(result, &de_ptp, "units", params->units);
    // создаем узел LuaFile
    createXml(result, &de_ptp, "LuaFile", params->luaScript);
    // создаем узел comment
    createXml(result, &de_ptp, "comment", params->comment);

    // прикрепляем дерево с параметрами процесса к общему
    parent->appendChild(de_ptp);
}

processParams* loadProcessParams(QDomNode dn_node)
{
    // выделяем память под структуру
    processParams* params = new processParams();

    QDomNode dn_nextNode = dn_node.firstChild();

    // перебираем узлы, пока не кончатся
    while (!dn_nextNode.isNull()) {

        // узел - processID
        if (dn_nextNode.nodeName() == "processID")
            params->processID = dn_nextNode.toElement().text().toInt();
        // узел - name
        if (dn_nextNode.nodeName() == "name")
            params->name = dn_nextNode.toElement().text();
        // узел - units
        if (dn_nextNode.nodeName() == "units")
            params->units = dn_nextNode.toElement().text();
        // узел - comment
        if (dn_nextNode.nodeName() == "comment")
            params->comment = dn_nextNode.toElement().text();
        // узел - LuaFile
        if (dn_nextNode.nodeName() == "LuaFile")
            params->luaScript = dn_nextNode.toElement().text();

        // переходим к следующему узлу
        dn_nextNode = dn_nextNode.nextSibling();
    }

    // возвращаем результат
    return params;
}

void saveProcessesParams(QDomDocument* result, QDomElement* parent, QVector<processParams*> params)
{
    // пишем данные в XML

    // создаем главный XML узел Process Types List
    QDomElement de_processtypes = result->createElement ("ProcessesList");

    // пишем в xml данные всех созданных типов процессов
    // с XML узлом Process Types List в качестве родительского
    for (int i = 0; i < params.size(); i++)
        saveProcessParams(result, &de_processtypes, params[i]);
        
    // добавляем главный узел в общее дерево
    parent->appendChild(de_processtypes);
}

QVector<processParams*> loadProcessesParams(QDomNode dn_node)
{
    // создаем массив параметров процессов
    QVector<processParams*> params;

    QDomNode dn_nextNode = dn_node.firstChild();

    // перебираем узлы, пока не закончатся
    while (!dn_nextNode.isNull ()) {

        // узел - Process
        if (dn_nextNode.nodeName () == "Process")
            // анализируем параметры процесса
            params.append(loadProcessParams(dn_nextNode));

        // переходим к следующему узлу
        dn_nextNode = dn_nextNode.nextSibling ();
    }

    // возвращаем результат
    return params;
}

projectParams* loadProjectParams(QDomNode dn_node)
{
    // выделяем память под структуру
    projectParams* params = new projectParams();
    
    QDomNode dn_nextNode = dn_node.firstChild();

    // перебираем узлы, пока не закончатся
    while (!dn_nextNode.isNull()) {

        // узел - autor
        if (dn_nextNode.nodeName() == "autor")
            params->autor = dn_nextNode.toElement().text();
        // узел - projectName
        if (dn_nextNode.nodeName() == "projectName")
            params->projectName = dn_nextNode.toElement().text();
        // узел - comment
        if (dn_nextNode.nodeName() == "comment")
            params->comment = dn_nextNode.toElement().text();

        // переходим к следующему узлу
        dn_nextNode = dn_nextNode.nextSibling();
    }

    // возвращаем результат
    return params;
}

void saveProjectParams(QDomDocument* result, QDomElement* parent, projectParams* params)
{
    // создаем узел Project, дочерними которго являются параметры проекта
    QDomElement de_project = result->createElement("Project");

    // создаем узел autor
    createXml(result, &de_project, "autor", params->autor);
    // создаем узел projectName
    createXml(result, &de_project, "projectName", params->projectName);
    // создаем узел comment
    createXml(result, &de_project, "comment", params->comment);

    // добавляем дерево параметров проекта к общему дереву
    parent->appendChild(de_project);
}

simulatorParams* loadSimulatorParams(QDomNode dn_node)
{
    // выделяем память для структуры
    simulatorParams* params = new simulatorParams();

    QDomNode dn_nextNode = dn_node.firstChild();

    // перебираем узлы, пока не закончатся
    while (!dn_nextNode.isNull ()) {

        // узел - version
        if (dn_nextNode.nodeName () == "version")
            params->version = dn_nextNode.toElement().text().toInt();

        // переходим к следующему узлу
        dn_nextNode = dn_nextNode.nextSibling();
    }

    // возвращаем результат
    return params;
}

void saveSimulatorParams(QDomDocument* result, QDomElement* parent, simulatorParams* params)
{
    // создаем узел Simulator, дочерними которого будут параметры симулятора
    QDomElement de_project = result->createElement("Simulator");
    
    // создаем узел version
    createXml(result, &de_project, "version", params->version);

    // прикрепляем дерево параметров симулятора к общему дереву
    parent->appendChild(de_project);
}

dataParams* loadDataParams(QString projectXML)
{
    // выделяем память для структур
    dataParams* params = new dataParams();
    envParams* env = new envParams();
    projectParams* project = new projectParams();
    simulatorParams* simulator = new simulatorParams();
    QVector<nodeParams*> nodes;
    QVector<nodeTypeParams*> nodeTypes;
    QVector<processParams*> processes;

    // создаем объект файла
    QFile file(projectXML);

    QDomDocument dd_doc;

    QString errorStr;
    int errorLine;
    int errorColumn;

    // читаем файл
    file.open(QFile::ReadOnly);
    QString content = file.readAll();

    // переносим текстовую информацию из файла в объекты XML
    if (!dd_doc.setContent(content, true, &errorStr, &errorLine, &errorColumn)) {
        qDebug() << "Bad XML file";
        return NULL;
    }

    // переходим к данным XML
    QDomElement de_root = dd_doc.documentElement();
    QDomNode dn_node = de_root.firstChild();

    // перебираем узлы, пока не закончатся
    while (!dn_node.isNull()) {

        // узел - Simulator
        if (dn_node.nodeName() == "Simulator")
            // получаем параметры симулятора
            simulator = loadSimulatorParams(dn_node);

        // узел - Project
        if (dn_node.nodeName() == "Project")
            // получаем параметры проекта
            project = loadProjectParams(dn_node);

        // узел - Env
        if (dn_node.nodeName() == "Env")
            // получаем параметры среды
            env = loadEnvParams(dn_node);

        // узел - ProcessesList
        if (dn_node.nodeName() == "ProcessesList")
            // получаем параметры процессов
            processes = loadProcessesParams(dn_node);

        // узел - NodeTypesList
        if (dn_node.nodeName() == "NodeTypesList")
            // получаем параметры типов узлов БСС
            nodeTypes = loadNodeTypesParams(dn_node);

        // узел - NodesList
        if (dn_node.nodeName() == "NodesList")
            // получаем параметры узлов БСС
            nodes = loadNodesParams(dn_node);

        // переходим к следующему узлу XML
        dn_node = dn_node.nextSibling();
    }

    // закрываем файл
    file.close();

    // заполняем структуру с данными
    params->env = env;
    params->project = project;
    params->simulator = simulator;
    params->processes = processes;
    params->nodeTypes = nodeTypes;
    params->nodes = nodes;

    // возвращаем результат
    return params;
}

void saveDataParams(QString projectXML, dataParams* params)
{
    // создаем объект файла
    QFile file(projectXML);
    
    // создаем объект с данными XML
    // в него сначала будут записаны все данные, потом он записывается в файл
    QDomDocument result;
    // создаем узел main, дочерними которого будут все остальные узлы
    QDomElement de_resultElement = result.createElement("main");

    // сохраняем параметры симулятора
    saveSimulatorParams(&result, &de_resultElement, params->simulator);
    // сохраняем параметры проекта
    saveProjectParams(&result, &de_resultElement, params->project);
    // сохраняем параметры среды
    saveEnvParams(&result, &de_resultElement, params->env);
    // сохраняем параметры типов узлов
    saveNodeTypesParams(&result, &de_resultElement, params->nodeTypes);
    // сохраняем параметры процессов
    saveProcessesParams(&result, &de_resultElement, params->processes);
    // сохраняем параметры узлов
    saveNodesParams(&result, &de_resultElement, params->nodes);

    // открываем файл на запись
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // пишем данные из объекта xml в файл
        
        QTextStream out(&file);

        QDomNode xmlNode = result.createProcessingInstruction("xml",
                                                              "version=\"1.0\" encoding=\"UTF-8\"");
        result.appendChild(xmlNode);
        result.appendChild(de_resultElement);
        out << result.toString(4);
    }
    
    file.close();
}
