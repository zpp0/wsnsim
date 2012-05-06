/***********************************************
 *
 * File: xml.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef XML_H
#define XML_H

#include "dataParams.h"

#include <QtXml>
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>

void createXml(QDomDocument* result, QDomElement* parent, QString XNodeName, QString XNodeValue);
void createXml(QDomDocument* result, QDomElement* parent, QString XNodeName, double XNodeValue);

// загрузка параметров из xml
envParams* loadEnvParams(QDomNode dn_node);
processParams* loadProcessParams(QDomNode dn_node);
QVector<processParams*> loadProcessesParams(QDomNode dn_node);
nodeTypeParams* loadNodeTypeParams(QDomNode dn_node);
QVector<nodeTypeParams*> loadNodeTypesParams(QDomNode dn_node);
sensorParams* loadSensorParams(QDomNode dn_node);
nodeParams* loadNodeParams(QDomNode dn_node);
simulatorParams* loadSimulatorParams(QDomNode dn_node);
projectParams* loadrojectParams(QDomNode dn_node);

dataParams* loadDataParams(QString projectXML);

// сохранение параметров в xml
void saveEnvParams(QDomDocument* result, QDomElement* parent, envParams* params);
void saveProcessParams(QDomDocument* result, QDomElement* parent, processParams* params);
void saveProcessesParams(QDomDocument* result, QDomElement* parent, QVector<processParams*> params);
void saveNodeTypeParams(QDomDocument* result, QDomElement* parent, nodeTypeParams* params);
void saveNodeTypesParams(QDomDocument* result, QDomElement* parent, QVector<nodeTypeParams*> params);
void saveSensorParams(QDomDocument* result, QDomElement* parent, sensorParams* params);
void saveSensorsParams(QDomDocument* result, QDomElement* parent, QVector<sensorParams*> params);
void saveNodeParams(QDomDocument* result, QDomElement* parent, nodeParams* params);
void saveNodesParams(QDomDocument* result, QDomElement* parent, QVector<nodeParams*> params);
void saveSimulatorParams(QDomDocument* result, QDomElement* parent, simulatorParams* params);
void saveProjectParams(QDomDocument* result, QDomElement* parent, projectParams* params);
void saveDataParams(QString projectXML, dataParams* params);

#endif // XML_H
