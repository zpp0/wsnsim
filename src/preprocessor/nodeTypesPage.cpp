#include <QListWidgetItem>
#include <QMenu>

#include "nodeTypesPage.h"
#include "ui_nodeTypesPage.h"

#include "dataParams.h"

NodeTypesPage::NodeTypesPage() :
    m_ui(new Ui::NodeTypesPage)
{
    // инициализация интерфейса
    m_ui->setupUi(this);

    // выключаем кнопку добавления типа узлов
    m_ui->b_addNodeType->setDisabled(true);
    
    // сигналы

    // нажата кнопка добавить тип узлов
    connect(m_ui->b_addNodeType, SIGNAL(clicked()),
            this, SLOT(addNodeType()));

    // изменено поле имени типа узлов
    connect(m_ui->le_nodeTypeName, SIGNAL(textChanged(QString)),
            this, SLOT(textChanged(QString)));

    // вызвано контекстное меню в таблице
    connect(m_ui->list_nodeTypeNames, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(customContextMenuRequested(const QPoint &)));
}

NodeTypesPage::~NodeTypesPage()
{
    delete m_ui;
}

NodeTypeParamsPage* NodeTypesPage::createNodeType(QString nt_name)
{
    // создаем элемент списка с именем nt_name
    QListWidgetItem *newNodeType = new QListWidgetItem(nt_name);
    // добавляем его в список типов узлов
    m_ui->list_nodeTypeNames->addItem(newNodeType);
    
    // очищаем поле имени нового типа узлов
    m_ui->le_nodeTypeName->clear();

    // создаем страницу с параметрами нового типа узла
    NodeTypeParamsPage *p_new = new NodeTypeParamsPage(nt_name);

    // добавляем указатель на страницу и имя типа узла в хеш массив
    m_h_ntpPages.insert(nt_name, p_new);

    // создаем событие тип узлов добавлен
    emit nodeTypeAdded(p_new, nt_name);

    return p_new;
}

void NodeTypesPage::addNodeType(QString nt_name)
{
    createNodeType(nt_name);
}


void NodeTypesPage::addNodeType(nodeTypeParams* params)
{
    if (params->name == NULL)
        return;

    // берем параметры из структуры
    QString nt_name = params->name;

    NodeTypeParamsPage *p_new = createNodeType(nt_name);
        
    // устанавливаем параметры типа
    p_new->setParams(params);
}


void NodeTypesPage::addNodeType()
{
    // получаем имя типа процесса
    QString nt_name = m_ui->le_nodeTypeName->text();
    createNodeType(nt_name);
}

void NodeTypesPage::textChanged(const QString text)
{
    // текст в строке имени типа добавляемого узла редактировался
    // блокируем кнопку "добавить", если имя типа узла не задано    
    if ((text == "")
        // FUCK! IDIODIC QT!
        || (m_h_ntpPages.keys().indexOf(text) != -1))
        m_ui->b_addNodeType->setEnabled(false);
    else 
        m_ui->b_addNodeType->setEnabled(true);
}

void NodeTypesPage::customContextMenuRequested(const QPoint &p)
{
    // обработчик события вызвано контестное меню в списке

    // создаем объект меню
    QMenu menu(m_ui->list_nodeTypeNames);
    
    // добавляем в него элемент Remove
    menu.addAction(m_ui->actionRemove);
    // menu.addAction (m_ui->actionRename);

    // пользователь нажимает на кнопку в меню, получаем действие, которое он хочет выполнить
    QAction *a = menu.exec(mapToGlobal(p));

    // получаем элемент списка, над которым пользователь вызвал меню
    QListWidgetItem *li_nt_name = m_ui->list_nodeTypeNames->itemAt(p);

    // если это не элемент списка
    if (li_nt_name == NULL)
        // ничего не делаем
        return;

    // если выбрано действие remove
    if (a == m_ui->actionRemove)
        // удаляем элемент списка
        removeNodeType(li_nt_name);

}

void NodeTypesPage::removeNodeType(QListWidgetItem * li_nt_name)
{
    // удаление элемента списка

    // получаем имя элемента списка
    QString nt_name = li_nt_name->text();

    // получаем из хеш массива указатель на страницу с параметрами типа узлов
    NodeTypeParamsPage* page = m_h_ntpPages.value(nt_name);
    // удаляем ее их хеш массива
    m_h_ntpPages.remove(nt_name);

    // удаляем элемент из списка
    m_ui->list_nodeTypeNames->removeItemWidget(li_nt_name);
    
    delete li_nt_name;

    // создаем событие удален тип узлов
    emit nodeTypeRemoved(page, nt_name);
}

void NodeTypesPage::removeNodeTypes()
{
    // очищение списка типов узлов

    // получаем количество типов узлов
    int rowCount = m_ui->list_nodeTypeNames->count();

    // для всех элементов списка
    for (int i = 0; i < rowCount; i++)
        // удаляем элемент
        removeNodeType(m_ui->list_nodeTypeNames->item(0));
}

QVector<nodeTypeParams*> NodeTypesPage::getParams()
{
    QVector<nodeTypeParams*> nodeTypes;

    // получаем список страниц с параметрами типов узлов
    QList<NodeTypeParamsPage*> list_ntpPages = m_h_ntpPages.values();

    // для всех страниц с параметрами типов узлов
    for (int i = 0; i < list_ntpPages.size(); i++) {
        // получаем их параметры и добавляем в массив
        nodeTypeParams* params = list_ntpPages[i]->getParams();
        params->nodeTypeID = i;
        nodeTypes.append(params);
    }

    // возвращаем результат
    return nodeTypes;
}

void NodeTypesPage::setParams(QVector<nodeTypeParams*> nodeTypes)
{
    if (nodeTypes.size() > 0) {
        // для всех параметров процессов из массива
        for (int i = 0; i < nodeTypes.size(); i++)
            if (nodeTypes[i] != NULL)
                // создаем процесс с параметрами из массива
                addNodeType(nodeTypes[i]);
    }
}
