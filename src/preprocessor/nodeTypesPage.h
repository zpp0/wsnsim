#ifndef NODETYPESPAGE_H
#define NODETYPESPAGE_H

#include <QGroupBox>
#include <QString>
#include <QListWidgetItem>

#include "nodeTypeParamsPage.h"
#include "dataParams.h"

namespace Ui {
    class NodeTypesPage;
}

class NodeTypesPage : public QGroupBox
{
    Q_OBJECT

public:
    NodeTypesPage();
    virtual ~NodeTypesPage();

    // очищение списка типов узлов
    void removeNodeTypes();

    // получить параметры типов узлов
    QVector<nodeTypeParams*> getParams();
    // установить параметры типов узлов
    void setParams(QVector<nodeTypeParams*> params);
    
private:
    Ui::NodeTypesPage *m_ui;
    
    NodeTypeParamsPage* createNodeType(QString nt_name);

    // хеш массив имен типов узлов и указателей на страницы параметров этих типов
    QHash <QString, NodeTypeParamsPage*> m_h_ntpPages;
    
signals:
    // сигналы

    // добавлен тип узла
    void nodeTypeAdded(NodeTypeParamsPage *n_page, QString name);
    // удален тип узла
    void nodeTypeRemoved(NodeTypeParamsPage *n_page, QString name);
    // изменено имя типа узла
    void nodeTypeChanged(QString oldName, QString newName);
                                           
private slots:
    // обработчики
    // обработчик события добавлен тип узла
    void addNodeType();
    void addNodeType(QString nt_name);
    void addNodeType(nodeTypeParams* params);
    // обработчик события изменен текст в поле имени типа узла
    void textChanged(const QString text);
    // обработчик события вызвано контекстное меню
    void customContextMenuRequested(const QPoint &p);
    // обработчик события удален тип узлов
    void removeNodeType(QListWidgetItem * li_nt_name);
};

#endif // NODETYPESPAGE_H
