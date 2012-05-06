#ifndef NODESPAGE_H
#define NODESPAGE_H

#include <QGroupBox>
#include <QString>
#include <QVector>

#include "nodeTypeParamsPage.h"

#include "dataParams.h"

namespace Ui {
    class NodesPage;
}

class NodesPage : public QGroupBox
{
    Q_OBJECT

public:
    NodesPage();
    virtual ~NodesPage();

    // получение массива структур параметров узлов
    QVector<nodeParams*> getParams();
    // запис данных из массива параметров узлов
    void setParams(QVector<nodeParams*> nodes);
                        
public slots:
    // обработка соботия добавления типа узлов
    void nodeTypeAdded(NodeTypeParamsPage* p, QString nt_name);
    // обработка события удаления типа узлов
    void nodeTypeRemoved(NodeTypeParamsPage* p, QString nt_name);
    // обработка события изменения размеров среды
    void envSizeChanged(double value, int coord);

private slots:
    // добавление узлов в таблицу при нажатии кнопки "добавить узлы"
    // количество узлов берется из интерфейса
    void createNodes();
    // очистить таблицу при нажатии кнопки "очистить"
    void cleanTable();

    // выбрана ячейка таблицы
    // если выбрана ячейка из первого столбца, заменяем элемент таблицы на combobox
    // или наоборот
    void currentCellChanged(int currentRow,
                            int currentColumn,
                            int previousRow,
                            int previousColumn);
    
private:
    // размеры среды
    double m_envSize[3];
    
    // номер активной строки
    int m_activeRow;
    
    Ui::NodesPage *m_ui;

    // добавление в таблицу узла с параметрами из аргументов
    void createNode(QString nt_name,
                    quint16 id,
                    double xcoord = 0.0,
                    double ycoord = 0.0,
                    double zcoord = 0.0,
                    bool random = false);

    // деактивация выбранной ячейки
    void deactivateCell();
    // активация ячейки, номер строки из аргумента
    void activateCell(int row);

    // массив имен типов узлов
    // нужен для определения их ID
    QVector<QString> m_nodeTypes;
};

#endif // NODESPAGE_H
