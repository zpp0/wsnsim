/***********************************************
 *
 * File: env.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <cmath>
#include <iostream>

#include "env.h"
#include "node.h"
#include "log.h"
#include "logevent.h"
#include "event.h"
#include "nodeEventMovement.h"
#include "nodeEventOn.h"
#include "message.h"
#include "luaHandler.h"
#include "channel.h"
#include "api.h"

static QHash<int, QString> envCoordsInfo;

QHash<int, channel*> env::m_h_channels;
QHash<channel*, QVector<node*> > env::m_h_nodeChannel;

quint64 env::m_maxSimulatorWorkTime;

quint64 env::m_mainTime;

lua_State* env::m_lua;

double env::m_size[3];

// глобальная очередь событий
// QMultiMap<quint64, event*> env::m_eventQueue;
QVector<event*> env::m_eventQueue;
// список зарегистрированных узлов
QVector<node*> env::m_nodes;
QVector<node*> env::m_movementNodes;
// QHash<quint16, node*> env::m_h_nodes;
// хеш массив ID узла -> его координаты
QHash<node*, double*> env::m_h_nodesCoords;
// массив типов узлов
QHash<quint16, nodeType*> env::m_h_nodeTypes;
// массив процессов
// QHash<quint16, process*> env::m_h_processes;
QHash<quint16, QString> env::m_h_processes;

void env::setEnv(envParams* params)
{
    m_mainTime = 0;
    
    m_lua = luaHandler::openLuaInterface();

    m_size[0] = params->size[0];
    m_size[1] = params->size[1];
    m_size[2] = params->size[2];
    m_maxSimulatorWorkTime = params->maxSimulatorWorkTime;

    switch (params->timeUnits) {
    case months:
        m_maxSimulatorWorkTime *= 30;
    case w:
        m_maxSimulatorWorkTime *= 7;
    case d:
        m_maxSimulatorWorkTime *= 24;
    case h:
        m_maxSimulatorWorkTime *= 60;
    case m:
        m_maxSimulatorWorkTime *= 60;
    case s:
        m_maxSimulatorWorkTime *= 1000;
    case ms:
        m_maxSimulatorWorkTime *= 1000;
    default:
        break;
    }
    
    envCoordsInfo[0] = "x";
    envCoordsInfo[1] = "y";
    envCoordsInfo[2] = "z";
    
    for (int i = 11; i <= 26; i++)
        m_h_channels[i] = new channel(2405 + 5 * (i - 11));
}

void env::setProcesses(QVector<processParams*>& processes)
{
    for (int i = 0; i < processes.size(); i++) {
        // создаем процесс
        // process* procNew = new process(processes[i]);
        // добавляем его в среду
        m_h_processes.insert(processes[i]->processID, processes[i]->name);
    }
}

void env::setNodeTypes(QVector<nodeTypeParams*>& nodeTypes)
{
    for (int i = 0; i < nodeTypes.size(); i++) {
        // создаем новый тип узлов с этим id
        nodeType* nt_new = new nodeType(nodeTypes[i]);
        // регистрируем в среде новый тип узлов
        m_h_nodeTypes.insert(nodeTypes[i]->nodeTypeID, nt_new);
    }
}

void env::setNodes(QVector<nodeParams*>& nodes)
{
    // создаем узлы с параметрами из переданного массива
    // запоминаем созданные узлы и их координаты в объекте среды
    // если для какого-то узла не существует типа узлов, кидаем исключение

    // для всех параметров узлов из массива
    for (int i = 0; i < nodes.size(); i++) {
        // если не существует типа узлов с нужным ID
        if (m_h_nodeTypes[nodes[i]->nodeTypeID] == NULL) {
            // выбрасываем исключение
            std::cerr << "Error:: Node " << i
                      << " request output NodeTyoe " << nodes[i]->nodeTypeID
                      << std::endl << "Exit." << std::endl;
            exit(1);
        }
        
        // создаем узел
        node* nodeNew = new node(nodes[i]->nodeID, m_h_nodeTypes[nodes[i]->nodeTypeID]);

        // получаем координаты узла
        double* coords = new double[3];
        
        // если координаты не случайные, то получаем их из структуры параметров
        if (nodes[i]->random == false) {
            // проверяем на совподение координат текущего узла с уже созданными
            int nodeIdWithSameCoord = isSameCoords(nodes[i]->coord);
            // если совпадений нет
            if (nodeIdWithSameCoord == -1)
                // получаем координаты нового узла из структуры параметров
                memmove(coords, nodes[i]->coord, sizeof(double) * 3);

            // если есть совпадение
            else {
                // пишем ошибку и выходим
                std::cerr << "Error: Nodes " << nodeIdWithSameCoord << " and " << i
                          << " have same coords: "  << nodes[i]->coord[0] << nodes[i]->coord[1] << nodes[i]->coord[2] << std::endl
                          << "Exit." << std::endl;

                exit(1);
            }
        }
                
        // иначе, генерируем их случайно
        else {
            do {
                for (int a = 0; a < 3; a++) {
                    qsrand(QDateTime::currentDateTime().toTime_t() + a + (int)nodeNew);
                    coords[a] = ((double)qrand() / RAND_MAX) * m_size[a];
                }
                // пока они совпадают с уже существующими
            } while (isSameCoords(coords) != -1);
        }
        
        const char* startUpTimeFuncName = nodeNew->getNodeType()->functionName(NODE_POWERON_TIME);

        quint64 startUpTime = 0;
        
        if (strcmp(startUpTimeFuncName, "") != 0) {

            QString randomSeedInit = "math.randomseed(" + QString::number(i) + ")";
            luaL_dostring(nodeNew->getLua(), randomSeedInit.toUtf8().constData());

            // вытаскиваем функцию процеесса lua 
            lua_getglobal(nodeNew->getLua(), startUpTimeFuncName);
            
            //вызов функции процесса
            if (lua_pcall(nodeNew->getLua(), 0, 1, 0) != 0)
                std::cerr << "Warning: node " << nodeNew->getMAC()
                          << " start up error " << std::endl;
            
            startUpTime = lua_tonumber(nodeNew->getLua(), -1);
            lua_pop(nodeNew->getLua(), 1);

            // if (startUpTime > m_maxSimulatorWorkTime)
            // startUpTime = 0;
        }
        qDebug() << "create node";
        // std::cerr << "create event NodeOn on time " << startUpTime << " node " << nodeNew << " id " << nodeNew->getMAC() << std::endl;

        nodeEventOn* event = new nodeEventOn(startUpTime, nodeNew, coords);
        insertEventQueue(event);
    }
}

env::~env()
{
    luaHandler::closeLuaInterface(m_lua);

    QList<channel* > chanels = m_h_nodeChannel.keys();
    
    for (int i = 0; i < chanels.size(); i++)
        delete chanels[i];

    QList<double* > coords = m_h_nodesCoords.values();

    for (int i = 0; i < coords.size(); i++)
        delete coords[i];
    
    // for (int i = 0; i < m_vec_nodes.size(); i++)
        // delete m_vec_nodes[i];
}

void env::registerNode(node* newNode, double* coords)
{
    m_nodes += newNode;

    // std::cerr << "m_h_nodes.size() " << m_h_nodes.size() << std::endl;
    // сохраняем координаты узла
    m_h_nodesCoords[newNode] = coords;
    
    // std::cerr << "m_h_nodesCoords.size() " << m_h_nodesCoords.size() << std::endl;

    // добавляем узел на стандартный 11 радио канал
    m_h_nodeChannel[m_h_channels[11]] += newNode;

    if (newNode->getNodeType()->isMovement() == true)
        m_movementNodes += newNode;
    // std::cerr << "m_h_nodeChannel.size() " << m_h_nodeChannel[m_h_channels[11]].size() << std::endl;
}


int env::isSameCoords(double coord[3])
{
    // QList<node*> nodes = m_h_nodes.values();
    for (int i = 0; i < m_nodes.size(); i++)
        // проверяем координаты a-го узла с новым
        if ((m_h_nodesCoords[m_nodes[i]][0] == coord[0])
            && (m_h_nodesCoords[m_nodes[i]][1] == coord[1])
            && (m_h_nodesCoords[m_nodes[i]][2] == coord[2]))
            return i;
    return -1;
}

//методы для работы с очередью событий

// //добавляем в очередь
// void env::insertEventQueue(event* event)
// {
//     m_eventQueue.insert(event->startTime, event);
// }

// // извлекаем из очереди
// event* env::popEventQueue()
// {
//     // вытаскиваем первый элемент
//     if (m_eventQueue.isEmpty())
//         return NULL;

//     event* first = *(m_eventQueue.lowerBound(0));
//     m_eventQueue.remove(first->startTime, first);

//     return first;
// }

//добавляем в очередь
void env::insertEventQueue(event* event)
{
    //если очередь не пуста
    if (!m_eventQueue.isEmpty()) {
        
        //если в самый конец
        if (event->startTime >= m_eventQueue.last()->startTime)
            m_eventQueue.insert(m_eventQueue.size(), event);

        else if (event->startTime <= m_eventQueue[0]->startTime)
            m_eventQueue.insert(0, event);
        
        //иначе, ищем подходящее место
        else {
            for (int i = 0; i < m_eventQueue.size() - 1; i++) {
                if (event->startTime >= m_eventQueue[i]->startTime
                    && event->startTime <= m_eventQueue[i + 1]->startTime) {
                    m_eventQueue.insert(i + 1, event);
                    break;
                }
            }
        }
    }
    
    else
        m_eventQueue << event;
}

// извлекаем из очереди
event* env::popEventQueue()
{
    // вытаскиваем первый элемент
    if (m_eventQueue.isEmpty())
        return NULL;

    // event* first = m_eventQueue.takeFirst();
    // event* first = *(m_eventQueue.lowerBound(0));
    // m_eventQueue.remove(first->startTime, first);
    event* first = m_eventQueue.first();

    // удаляем этот элемент из очереди
    m_eventQueue.remove(0);

    return first;
}

channel* env::getChannel(int number)
{
    return m_h_channels[number];
}

//для управления наиболее значимыми переменными
void env::setMainTime(quint64 ms)
{
    m_mainTime = ms;
}

quint64 env::getMainTime()
{
    return m_mainTime;
}

quint64 env::getMaxSimulatorWorkTime()
{
    return m_maxSimulatorWorkTime;
}

//проверяем, слышат ли узлы друг друга
void env::nodesHearTest()
{
    // составляем матрицы достижимости узлов для всех каналов

    // получаем список существующих каналов
    QList<channel*> channels = m_h_channels.values();

    // для всех каналов
    for (int i = 0; i < channels.size(); i++)
        // если на канале есть узлы
        if (m_h_nodeChannel[channels[i]].size() > 0)
            // проверяем слышимость этих узлов в канале
            channels[i]->nodesHearTest(m_h_nodeChannel[channels[i]]);
}

bool env::moveNodes()
{
    // перемещаем узлы

    if (m_movementNodes.isEmpty())
        return false;

    // для всех узлов, которые могут перемещатсья
    for (int i = 0; i < m_movementNodes.size(); i++)
        // перемещаем его
        moveNode(m_nodes[i]);

    return true;
}

double env::distance(node* node1, node* node2)
{
    double *coord1 = m_h_nodesCoords[node1];
    double *coord2 = m_h_nodesCoords[node2];

    return sqrt(pow((coord2[0]-coord1[0]), 2)
                + pow((coord2[1]-coord1[1]), 2)
                + pow((coord2[2]-coord1[2]), 2));
}

void env::direction(node* node1, node* node2, double distance, double* nu, double* fi)
{
    // передвигаем начало системы координат к узлу node1

    double coord2Relative[3];
    // получаем координаты узла node2 в новой системе
    for (int i = 0; i < 3; i++)
        coord2Relative[i] = m_h_nodesCoords[node2][i] - m_h_nodesCoords[node1][i];

    // переводим координаты узла node2 в сферическую систему с центром в узле node1
    *nu = acos(coord2Relative[2] / distance) * 180.0 / M_PI;
    *fi = atan2(coord2Relative[1], coord2Relative[0]) * 180.0 / M_PI;
}

double* env::nodeCoordAtTime(node* node, quint64 time)
{
    // получаем координаты узла, которые должны быть во времени time

    // получаем текущие координаты
    double* coordOld = m_h_nodesCoords[node];

    // получаем указатель на виртуальную машину lua этого узла
    lua_State* luaVM = node->getLua();

    // подготавливаем к запуску функцию movement на lua
    lua_getglobal(node->getLua(), node->getNodeType()->functionName(NODE_MOVEMENT));

    // передаем в нее координаты и время
    lua_pushnumber(luaVM, coordOld[0]);
    lua_pushnumber(luaVM, coordOld[1]);
    lua_pushnumber(luaVM, coordOld[2]);
    lua_pushnumber(luaVM, time);

    // ERROR: если произойдет ошибка, то прога скорее всего сегфолтнется
    // вызываем функцию с 4 аргументами и 3 результатами
    if (lua_pcall(luaVM, 4, 3, 0) != 0) {
        
        // если возникли проблемы, выводим их в stderr
        // lua_tostring(luaVM, 0);
        std::cerr << "Warning: Node " << node->getMAC() << "move error from coords "
                  << coordOld[0] << coordOld[1] << coordOld[2]
                  << " at time " << time << std::endl;
    }

    // получаем новые координаты
    double* coordNew = new double[3];
    for (int i = 2; i >= 0; i--) {
        coordNew[i] = lua_tonumber(luaVM, -1);
        // std::cerr << coordNew[i] << " ";
        lua_pop(luaVM, 1);
    }
    
    // возвращаем результат
    return coordNew;
}

quint64 env::nodeTime(node* node, quint64 time, const char* function)
{
    lua_State* luaVM = node->getLua();
    QList<quint16> processesID = node->getNodeType()->clockProcessDependensiesIDList();

    // вытаскиваем функцию вычисления локального времени lua 
    lua_getglobal(luaVM, function);

    // передаем в нее время
    lua_pushnumber(luaVM, time);

    // и значения всех требуемых процессов
    for (int i = 0; i < processesID.size(); i++) {
        // измеряем процесс
        double proc = env::measure(node, processesID[i]);

        // передаем его в lua
        lua_pushnumber(luaVM, proc);
    }

    // вызов функции
    if (lua_pcall(luaVM, processesID.size() + 1, 1, 0) != 0)
        std::cerr << "Warning: node " << node->getMAC() << " computing " << function
                  << " error at time " << m_mainTime << std::endl;

    // получаем результат
    quint64 result = lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    // возвращаем результат
    return result;
}

quint64 env::nodeLocaltime(node* node, quint64 globalTime)
{
    return nodeTime(node, globalTime, "localtime");
}

quint64 env::nodeGlobaltime(node* node, quint64 localTime)
{
    return nodeTime(node, localTime, "globaltime");
}

void env::moveNode(node* node)
{
    // перемещаем узел
    double* coordNew = nodeCoordAtTime(node, m_mainTime);

    // если новые координаты узла выходят за пределы среды
    for (int i = 0; i < 3; i++) {
        if ((coordNew[i] > m_size[i]) || (coordNew[i] < 0)) {

            // оповещаем об этом пользователя
            std::cerr << "Warning: node " << node->getMAC() << " going out environment with "
                      << coordNew[i] << envCoordsInfo[i].toStdString() << " coord "
                      << " at time " << m_mainTime << std::endl;
            
            coordNew[i] = m_size[i];
        }
    }

    int nodeID;
    // узел наехал на другой
    if ((nodeID = isSameCoords(coordNew)) != -1) {
        // оповещаем об этом пользователя
        std::cerr << "Warning: node " << node->getMAC()
                  << " run into node " << nodeID
                  << " at time " << m_mainTime << std::endl;
    }

    double* coordOld = m_h_nodesCoords[node];

    // если координаты изменились хотя бы по 1 оси
    for (int i = 0; i < 3; i++) {
        if (abs(coordNew[i] - coordOld[i]) > 0.001) {

            // заменяем старые координаты новыми
            memmove(coordOld, coordNew, sizeof(double) * 3);

            // пишем в лог
            qDebug() << "create movement event";

            nodeEventMovement* event =
                new nodeEventMovement(env::getMainTime(),
                                      node,
                                      coordOld);

            event->action();

            break;
        }
    }
    delete[] coordNew;
}

double env::measure(node* node, quint16 processID)
{
    char *w = m_h_processes[processID].toLocal8Bit().data();
    // вытаскиваем функцию процеесса lua 
    lua_getglobal(m_lua, w);

    double* coord = m_h_nodesCoords[node];

    // передаем в нее координаты и время
    lua_pushnumber(m_lua, coord[0]);
    lua_pushnumber(m_lua, coord[1]);
    lua_pushnumber(m_lua, coord[2]);
    lua_pushnumber(m_lua, m_mainTime);

    //вызов функции процесса
    if (lua_pcall(m_lua, 4, 1, 0) != 0)
        std::cerr << "Warning: process " << m_h_processes[processID].toStdString() << " measure error "
                  << " at coordinates " << coord[0] << coord[1] << coord[2]
                  << " time " << m_mainTime << std::endl;

    double value = lua_tonumber(m_lua, -1);
    lua_pop(m_lua, 1);
    
    return value;

    // FIXME: request undefined sensor
    // return m_h_processes[processID]->measure(m_h_nodesCoords[node], m_mainTime);
}
