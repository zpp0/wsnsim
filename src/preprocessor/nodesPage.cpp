#include <QtGui>

#include "nodeTypeParamsPage.h"

#include "nodesPage.h"
#include "ui_nodesPage.h"

#include "xml.h"
#include "dataParams.h"

NodesPage::NodesPage() :
    m_ui(new Ui::NodesPage)
{
    m_ui->setupUi (this);

    // устанавливаем стандартные размеры среды
    // TODO: получать начальные значения из окна с размерами среды
    m_envSize[0] = 1.0;
    m_envSize[1] = 1.0;
    m_envSize[2] = 1.0;

    m_activeRow = -1;

    m_ui->t_nodes->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    // m_ui->t_nodes->setContextMenuPolicy(Qt::CustomContextMenu);

    // сигналы

    // нажата кнопка добавить узлы
    connect(m_ui->b_addNodes, SIGNAL(clicked()),
             this, SLOT(createNodes()));

    // нажата кнопка очистить таблицу
    connect(m_ui->b_clear, SIGNAL(clicked()),
             this, SLOT(cleanTable()));

    // нажат элемент таблицы
    connect(m_ui->t_nodes, SIGNAL(currentCellChanged(int, int, int, int)),
             this, SLOT(currentCellChanged(int, int, int, int)));
}

NodesPage::~NodesPage()
{
    delete m_ui;
}

void NodesPage::nodeTypeAdded(NodeTypeParamsPage* p, QString nt_name)
{
    // обрабатываем сигнал "создан новый тип узлов"

    // запоминаем тип узлов в массив
    m_nodeTypes.append(nt_name);
    
    // если это первый тип узлов, активируем кнопку "добавить узлы"
    // иначе, кнопка уже активирована
    if (m_ui->cb_nodeTypes->count() == 0)
        m_ui->b_addNodes->setEnabled(true);

    // добавляем в ComboBox добавления узлов имя нового типа узлов
    m_ui->cb_nodeTypes->addItem(nt_name);

    // если был активирован элемент таблицы, обновляем его combobox
    if (m_activeRow >= 0) {
        QComboBox *cb_n_Type = (QComboBox*) m_ui->t_nodes->cellWidget(m_activeRow, 0);
        cb_n_Type->addItem(nt_name);
    }
}

void NodesPage::nodeTypeRemoved (NodeTypeParamsPage* p, QString nt_name)
{
    // обрабатываем сигнал "тип узлов удален"
    int index = m_ui->cb_nodeTypes->findText(nt_name);

    // если это был последний тип узлов
    if (m_ui->cb_nodeTypes->count() == 1)
        // блокируем кнопку добавления узлов в таблицу
        m_ui->b_addNodes->setEnabled(false);

    // удаляем тип узлов из ComboBox добавления узлов
    m_ui->cb_nodeTypes->removeItem(index);

    // деактивируем ячейку
    deactivateCell();

    m_ui->t_nodes->setCurrentCell(-1, -1);
    
    // удаляем узлы с удаляемым типом
    // NOTE: после удаления строки через removeRow количество строк изменится

    // поиск в таблице элементов, с подходящим именем типа узла
    QList<QTableWidgetItem *> list_nt_delete = m_ui->t_nodes->findItems(nt_name, Qt::MatchExactly);
    QList<QTableWidgetItem *>::iterator i;

    // удаляем их
    for (i = list_nt_delete.begin(); i != list_nt_delete.end(); i++) {
        int row = m_ui->t_nodes->row((*i));
        m_ui->t_nodes->removeRow(row);
    }

    // удаляем тип узлов из массива
    int nt_index = m_nodeTypes.indexOf(nt_name);
    m_nodeTypes.remove(nt_index);
}

void NodesPage::createNodes()
{
    // заполняем таблицу узлами

    // получаем количество добавляемых узлов
    int n_count = m_ui->sb_nodesCount->value();
    // получаем текущее количество узлов
    int rowCount = m_ui->t_nodes->rowCount();
    // получаем имя типа добавляемых узлов
    QString nt_name = m_ui->cb_nodeTypes->currentText();

    // // создаем строки в таблице для новых узлов
    // m_ui->t_nodes->setRowCount (rowCount + n_count);

    // для всех создаваемых узлов
    for (int i = 0; i < n_count; i++)
        createNode(nt_name, rowCount + i);
}

void NodesPage::createNode(QString nt_name,
                                   quint16 id,
                                   double xcoord,
                                   double ycoord,
                                   double zcoord,
                                   bool random)
{
    if (nt_name == "")
        return;

    m_ui->t_nodes->insertRow(id);
    
    // создаем Label с именем типа узла
    // при нажатии на этот label появится ComboBox
    QTableWidgetItem *ti_nt_name = new QTableWidgetItem(nt_name);
    // QLabel *l_nt_name = new QLabel (nt_name);

    // создаем SpinBox'ы для задания координат узлов
    // и указываем максимальные значения координат по осям

    // по оси x
    QDoubleSpinBox *dsb_n_coordX = new QDoubleSpinBox();
    dsb_n_coordX->setButtonSymbols(QAbstractSpinBox::NoButtons);
    dsb_n_coordX->setMaximum(m_envSize[0]);
    dsb_n_coordX->setValue(xcoord);

    // по оси y
    QDoubleSpinBox *dsb_n_coordY = new QDoubleSpinBox();
    dsb_n_coordY->setButtonSymbols(QAbstractSpinBox::NoButtons);
    dsb_n_coordY->setMaximum(m_envSize[1]);
    dsb_n_coordY->setValue(ycoord);

    // по оси z
    QDoubleSpinBox *dsb_n_coordZ = new QDoubleSpinBox();
    dsb_n_coordZ->setButtonSymbols(QAbstractSpinBox::NoButtons);
    dsb_n_coordZ->setMaximum(m_envSize[2]);
    dsb_n_coordZ->setValue(zcoord);
        
    // кнопка "случайное размещение"
    // ее включение блокирует SpinBox'ы с координатами
    QCheckBox *chb_random = new QCheckBox("да");

    // соединяем кнопку "случайное размещение" с включением/выключением SpinBox'ов
    connect(chb_random, SIGNAL(clicked(bool)),
             dsb_n_coordX, SLOT(setDisabled(bool)));

    connect(chb_random, SIGNAL(clicked(bool)),
             dsb_n_coordY, SLOT(setDisabled(bool)));

    connect(chb_random, SIGNAL(clicked(bool)),
             dsb_n_coordZ, SLOT(setDisabled(bool)));

    // добавляем созданные элементы в таблицу
    m_ui->t_nodes->setItem(id, 0, ti_nt_name);
    // m_ui->t_nodes->setCellWidget (id, 0, l_nt_name);
    m_ui->t_nodes->setCellWidget(id, 1, dsb_n_coordX);
    m_ui->t_nodes->setCellWidget(id, 2, dsb_n_coordY);
    m_ui->t_nodes->setCellWidget(id, 3, dsb_n_coordZ);
    m_ui->t_nodes->setCellWidget(id, 4, chb_random);

    // если необходимо, включаем случайные координаты узлов
    if (m_ui->cb_randomCoord->checkState() == Qt::Checked
        || random == true)
        chb_random->click();
}

void NodesPage::envSizeChanged(double value, int coord)
{
    // обрабатываем сигнал "изменен размер среды"
    // устанавливаем размер по оси coord равным value
    m_envSize[coord] = value;

    // определяем количество узлов
    int nodesCount = m_ui->t_nodes->rowCount();

    // изменяем максимальные значения координат узлов по измененной оси в таблице узлов
    for (int i = 0; i < nodesCount; i++) {
        QDoubleSpinBox *dsb_envSize = (QDoubleSpinBox*) m_ui->t_nodes->cellWidget(i, coord + 1);
        dsb_envSize->setMaximum(value);
    }
}

void NodesPage::cleanTable()
{
    // очищаем содержимое таблицы
    m_ui->t_nodes->clearContents();
    m_ui->t_nodes->setRowCount(0);
}

void NodesPage::currentCellChanged(int currentRow,
                                           int currentColumn,
                                           int previousRow,
                                           int previousColumn)
{

    // событие перехода от одной ячейки таблицы к другой

    // если перешли от ячейки первого столбца
    if (previousColumn == 0)
        // деактивируем ее
        deactivateCell();

    // если переходим к ячейке таблицы из первого столбца
    if (currentColumn == 0)
        // активируем ее
        activateCell(currentRow);
}

void NodesPage::deactivateCell()
{
    // если была выбрана ячейка
    if (m_activeRow >= 0) {

        // удаляем имеющийся там ComboBox
        QComboBox *cb_n_Type = (QComboBox*) m_ui->t_nodes->cellWidget(m_activeRow, 0);
        m_ui->t_nodes->removeCellWidget(m_activeRow, 0);

        // добавляем туда обычный элемент таблицы
        
        // QLabel *l_n_Type = new QLabel (cb_n_Type->currentText ());
        QTableWidgetItem *ti_n_Type = new QTableWidgetItem(cb_n_Type->currentText());
        m_ui->t_nodes->setItem(m_activeRow, 0, ti_n_Type);
        // m_ui->t_nodes->setCellWidget (m_activeRow, 0, l_n_Type);

        // теперь активная строка отсутствует
        m_activeRow = -1;
        
        delete cb_n_Type;
    }
}

void NodesPage::activateCell (int row)
{
    if (m_activeRow == -1) {
        // создаем ComboBox с типами узлов
        QComboBox *cb_n_Type = new QComboBox();
        int nt_nameIndex = m_ui->cb_nodeTypes->currentIndex();
        
        // получаем количество типов узлов
        int nt_count = m_ui->cb_nodeTypes->count();
        
        // заполняем ComboBox уже существующими типами
        for (int j = 0; j < nt_count; j++)
            cb_n_Type->addItem (m_ui->cb_nodeTypes->itemText(j));

        // устанавливаем в combobox нужный тип узла
        cb_n_Type->setCurrentIndex(nt_nameIndex);

        // удаляем старый элемент таблицы
        QTableWidgetItem *ti_n_Type = m_ui->t_nodes->item(row, 0);
        // QLabel *l_n_Type = (QLabel*) m_ui->t_nodes->cellWidget (row, 0);

        m_ui->t_nodes->setCellWidget(row, 0, cb_n_Type);

        // теперь есть активная строка
        m_activeRow = row;

        delete ti_n_Type;
    }
}

void NodesPage::setParams(QVector<nodeParams*> nodes)
{
    // получаем параметры узлов из структуры
    // очищаем таблицу
    cleanTable();
        
    // для всех элементов массива узлов
    for (int i = 0; i < nodes.size(); i++)
        // создаем узлы с параметрами из массива
        createNode(m_nodeTypes[nodes[i]->nodeTypeID],
                   nodes[i]->nodeID,
                   nodes[i]->coord[0],
                   nodes[i]->coord[1],
                   nodes[i]->coord[2],
                   nodes[i]->random);
}

QVector<nodeParams*> NodesPage::getParams()
{
    // создаем массив параметров узлов
    QVector<nodeParams*> nodes;
    
    // получаем количество узлов
    int nodesCount = m_ui->t_nodes->rowCount();

    // деактивируем ячейку, если она была активирована
    deactivateCell();

    // для всех узлов из таблицы
    for (int i = 0; i < nodesCount; i++) {
        // получаем их параметры
        // название типа
        QTableWidgetItem* ti_nt_name = m_ui->t_nodes->item(i, 0);
        // координаты по x
        QDoubleSpinBox* dsb_x = (QDoubleSpinBox*) m_ui->t_nodes->cellWidget(i, 1);
        // по y
        QDoubleSpinBox* dsb_y = (QDoubleSpinBox*) m_ui->t_nodes->cellWidget(i, 2);
        // и по z
        QDoubleSpinBox* dsb_z = (QDoubleSpinBox*) m_ui->t_nodes->cellWidget(i, 3);
        // случайное размещение?
        QCheckBox* cb_random = (QCheckBox*) m_ui->t_nodes->cellWidget(i, 4);

        // выделяем память для структуры
        nodeParams* params = new nodeParams();

        // записываем параметры
        // ID узла
        params->nodeID = i;
        // ID типа узла
        params->nodeTypeID = m_nodeTypes.indexOf(ti_nt_name->text());
        // координаты
        params->coord[0] = dsb_x->value();
        params->coord[1] = dsb_y->value();
        params->coord[2] = dsb_z->value();
        // случайно ли размещается
        params->random = cb_random->isChecked();
        
        // добавляем структуру в массив
        nodes.append(params);
    }

    // возвращаем узлы
    return nodes;
}
