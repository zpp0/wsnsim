/***********************************************
 *
 * File: env.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef ENV_H
#define ENV_H

#include <QString>
#include <QVector>
#include <QHash>

#include "event.h"
#include "node.h"
#include "message.h"
#include "luaHandler.h"
#include "channel.h"
/* #include "process.h" */

#include "dataParams.h"

/*
    класс Среды. существует в единственным экзмепляре.
*/

class env
{
public:
    // СЛУЖЕБНОЕ
    ~env();

    static void setEnv(envParams* params);
    // получаем параметры процессов
    static void setProcesses (QVector<processParams*>& processes);
    // получаем параметры типов узлов
    static void setNodeTypes(QVector<nodeTypeParams*>& nodeTypes);
    // получаем параметры узлов
    static void setNodes(QVector<nodeParams*>& nodes);

    static void registerNode(node* node, double* coords);
    
    // РЯД МЕТОДОВ ДЛЯ УПРВЛЕНИЯ ЗНАЧЕНИЯМИ НАИБОЛЕЕ ЗНАЧИМЫХ СВОЙСТВ КЛАССА
    // устанавливает значение глобального времени среды в микросекундах
    static void setMainTime(quint64 mc);
    // возвращает текущее время в микросекундах
    static quint64 getMainTime();
    
    // максимальное время работы симулятора
    static quint64 getMaxSimulatorWorkTime();

    /* // метод возаращает координаты узла с заданным ID */
    /* static double* getNodeCoord(node* node); */

    static double distance(node* node1, node* node2);
    
    static void direction(node* node1, node* node2, double distance, double* nu, double* fi);
    
    // МЕТОДЫ ДЛЯ УПРВЛЕНИЯ ОЧЕРЕДЬЮ СОБЫТИЙ
    // извлечь из очереди очередное событие
    // FIXME: перенести это в simulator?
    static event* popEventQueue();
    // вставляет событие в нужное по хранологии место в очереди
    static void insertEventQueue(event* event);

    static channel* getChannel(int number);
    
    // проверяем, слышат ли узлы друг друга
    static void nodesHearTest();
    static bool moveNodes();
            
    static quint64 nodeLocaltime(node* node, quint64 time);
    static quint64 nodeGlobaltime(node* node, quint64 time);

    static double measure(node* node, quint16 processID);
    
private:

    static QVector<node*>& nodesOnChannel(int number);

    static double* nodeCoordAtTime(node* node, quint64 time);

    static void moveNode(node* node);

    static int isSameCoords(double coord[3]);

    static quint64 nodeTime(node* node, quint64 time, const char* function);
    
    // СВОЙСТВА - ХАРАКТЕРИСТИКИ СРЕДЫ
    // максимально-возможное время работы симулятора
    static quint64 m_maxSimulatorWorkTime;
        
    // реальное время в системе
    static quint64 m_mainTime;

    static lua_State* m_lua;

    // размеры среды по 3 измерениям
    static double m_size[3];

    // глобальная очередь событий
    static QVector<event*> m_eventQueue;
    /* static QList<event*> m_eventQueue; */
    /* static QMultiMap<quint64, event*> m_eventQueue; */
    // список зарегистрированных узлов
    static QVector<node*> m_nodes;
    static QVector<node*> m_movementNodes;
    // хеш массив ID узла -> его координаты
    static QHash<node*, double*> m_h_nodesCoords;
    // массив типов узлов
    static QHash<quint16, nodeType*> m_h_nodeTypes;
    // массив процессов
    /* static QHash<quint16, process*> m_h_processes; */
    static QHash<quint16, QString> m_h_processes;

    static QHash<int, channel*> m_h_channels;
    static QHash<channel*, QVector<node*> > m_h_nodeChannel;
};

#endif // ENV_H
