// #ifdef WIN32
// #include <shellapi.h>
// #endif

#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QDesktopServices>

#include <QFileDialog>

#include "myspinbox.h"

#include "dataParams.h"
#include "api.h"

#include "nodeTypeParamsPage.h"
#include "ui_nodeTypeParamsPage.h"

NodeTypeParamsPage::NodeTypeParamsPage(QString nt_name) :
    m_ui(new Ui::NodeTypeParamsPage)
{
    // инициализируем интерфейс
    m_ui->setupUi(this);

    // заголовок GroupBox
    setTitle(nt_name);

    // запоминаем имя типа
    m_name = nt_name;

    m_ui->cb_depends->hide();

    m_ui->t_sensors->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    
    // столбец с названием функции растягивается по максимальной длине названия
    m_ui->t_notifyFunctions->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    // столбец с именем функции занимает все остальное место
    m_ui->t_notifyFunctions->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

    // столбец с названием функции растягивается по максимальной длине названия
    m_ui->t_controlFunctions->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    // столбец с именем функции занимает все остальное место
    m_ui->t_controlFunctions->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

    QList<QString> stdFunctions;
    // stdFunctions.append(NODE_POWERON_TIME);
    stdFunctions.append(NODE_INITIALISATION);
    stdFunctions.append(TIMER_INTERRUPT_HANDLER);
    stdFunctions.append(RADIO_INTERRUPT_HANDLER);
    
    for (int i = 0; i < stdFunctions.size(); i++)
        addFunctionLabel(stdFunctions[i]);

    addFunctionLabel(NODE_POWERON_TIME, 1);

    // сигналы

    // нажатие кнопки добавления зависимости
    connect(m_ui->b_addDepend, SIGNAL(clicked()),
            this, SLOT(createTimerDependence()));
    
    // нажатие кнопки открытия файла обрабатывает страница типа узла
    connect(m_ui->tb_luaFile, SIGNAL(clicked()),
            this, SLOT(chooseLuaProgrammFile()));
    
    // нажатие кнопки редактирования файла в редакторе обрабатывает страница типа узла
    connect(m_ui->tb_luaFileEdit, SIGNAL(clicked()),
            this, SLOT(openLuaProgrammFile()));

    
    // // нажатие кнопки открытия файла обрабатывает страница типа узла
    // connect(m_ui->tb_luaFileStartTime, SIGNAL(clicked()),
    //         this, SLOT(chooseLuaStartTimeFile()));
    
    // // нажатие кнопки редактирования файла в редакторе обрабатывает страница типа узла
    // connect(m_ui->tb_luaFileStartTimeEdit, SIGNAL(clicked()),
    //         this, SLOT(openLuaStartTimeFile()));
    
    // включение/выключение опции направленной антенны активирует/деактивирует строку для ввода файла
    connect(m_ui->cb_directionalAntenna, SIGNAL(clicked()),
            this, SLOT(toggleDirectionAntennaFunction()));

    // тоже самое для реалистичной модели таймера
    connect(m_ui->cb_realTimer, SIGNAL(clicked()),
            this, SLOT(toggleRealTimerFunction()));

    connect(m_ui->cb_realTimer, SIGNAL(clicked(bool)),
            m_ui->g_depends, SLOT(setEnabled(bool)));

    connect(m_ui->cb_realTimer, SIGNAL(clicked(bool)),
            m_ui->t_timerDepends, SLOT(setEnabled(bool)));
    
    // и возможности перемещения узлов
    connect(m_ui->cb_movement, SIGNAL(clicked()),
            this, SLOT(toggleMovementFunction()));
}

NodeTypeParamsPage::~NodeTypeParamsPage()
{
    delete m_ui;
}

void NodeTypeParamsPage::addTimerDependence(QString name)
{
    if (m_ui->cb_depends->count() == 0) {
        m_ui->cb_depends->show();
        m_ui->b_addDepend->setEnabled(true);
    }
    
    m_ui->cb_depends->addItem(name);
}

void NodeTypeParamsPage::removeTimerDependence(QString name)
{
    // удаляем зависимость
    // если она еще не выбрана, то удалим ее из combobox
    // если выбрана - надо найти ее в таблице и удалить оттуда
    
    // ищем в combobox подходящее название
    int index = m_ui->cb_depends->findText(name);
    
    // если нашли
    if (index != -1)
        // удаляем
        m_ui->cb_depends->removeItem(index);
    
    // если нету
    else {
        // смотрим элемент в таблице
        // он будет только 1
        QList<QTableWidgetItem*> items = m_ui->t_timerDepends->findItems(name, Qt::MatchFixedString);
        // получаем номер строки, в которой находится этот элемент
        int row = m_ui->t_timerDepends->row(items[0]);
        // удаляем строку
        m_ui->t_timerDepends->removeRow(row);
    }

    if (m_ui->cb_depends->count() == 0) {
        m_ui->cb_depends->hide();
        m_ui->b_addDepend->setEnabled(false);
    }
}

void NodeTypeParamsPage::removeSensor(QString name)
{
    // процесс удален, надо удалить датчик для него

    int row = pt_findRow(name);
    
    // удаляем элементы из таблицы

    // удаляем CheckBox
    QCheckBox *chb_sensorSet = (QCheckBox*) m_ui->t_sensors->cellWidget(row, 0);
    delete chb_sensorSet;
    
    // удаляем Label с именеи
    QLabel *l_units = (QLabel*) m_ui->t_sensors->cellWidget(row, 1);
    delete l_units;
    // удаляем SpinBox с нижней и верхней границей чувствительности
    MyDoubleSpinBox *msb_min = (MyDoubleSpinBox*) m_ui->t_sensors->cellWidget(row, 2);
    delete msb_min;
    
    MyDoubleSpinBox *msb_max = (MyDoubleSpinBox*) m_ui->t_sensors->cellWidget(row, 3);
    delete msb_max;

    // удаляем строку из таблицы
    m_ui->t_sensors->removeRow(row);

}

void NodeTypeParamsPage::addSensor(QString name)
{
    // процесс создан, надо добавить датчик для него

    // получаем текущее количество датчиков
    int rowCount = m_ui->t_sensors->rowCount();

    // создаем строки в таблице для нового датчика
    m_ui->t_sensors->setRowCount(rowCount + 1);

    // CheckBox для включения/выключения датчика в состав узла
    QCheckBox *chb_sensorSet = new QCheckBox(name);

    // Имя датчика и единицы измерения.
    QLabel *l_sensorUnits = new QLabel();

    // SpinBox с верхней границей чувствительного диапазона
    MyDoubleSpinBox *msb_maxBand = new MyDoubleSpinBox();
    msb_maxBand->setMaximum(1000.0);
    msb_maxBand->setButtonSymbols(QAbstractSpinBox::NoButtons);

    // SpinBox с нижней границей чувствительного диапазона
    MyDoubleSpinBox *msb_minBand = new MyDoubleSpinBox();
    msb_minBand->setMinimum(-1000.0);
    msb_minBand->setButtonSymbols(QAbstractSpinBox::NoButtons);

    // добавляем созданные элементы в таблицу
    m_ui->t_sensors->setCellWidget(rowCount, 0, chb_sensorSet);
    m_ui->t_sensors->setCellWidget(rowCount, 1, l_sensorUnits);
    m_ui->t_sensors->setCellWidget(rowCount, 2, msb_minBand);
    m_ui->t_sensors->setCellWidget(rowCount, 3, msb_maxBand);

    // деактивируем поля, пока пользователь не включит датчик на узле
    l_sensorUnits->setDisabled(true);
    msb_minBand->setDisabled(true);
    msb_maxBand->setDisabled(true);

    // сигналы

    // изменение значения нижней границы чувствительности обрататывает spinbox верхней границы
    connect(msb_minBand, SIGNAL(valueChanged(double)),
            msb_maxBand, SLOT(setMin(double)));

    // и наоборот
    connect(msb_maxBand, SIGNAL(valueChanged(double)),
            msb_minBand, SLOT(setMax(double)));

    // включение/выключения датчика активирует/деактивирует его поля в таблице
    connect(chb_sensorSet, SIGNAL(clicked(bool)),
            l_sensorUnits, SLOT(setEnabled(bool)));

    connect(chb_sensorSet, SIGNAL(clicked(bool)),
            msb_minBand, SLOT(setEnabled(bool)));

    connect(chb_sensorSet, SIGNAL(clicked(bool)),
            msb_maxBand, SLOT(setEnabled(bool)));
}

void NodeTypeParamsPage::processAdded(QString name)
{
    addSensor(name);
    addTimerDependence(name);
}

void NodeTypeParamsPage::processRemoved(QString name)
{
    removeSensor(name);
    removeTimerDependence(name);
    
    // событие изменены единицы измерений

}

void NodeTypeParamsPage::processUnitChanged(quint8 processID, QString units)
{
    // изменены единицы измерений процесса, надо изменить их на соответствующем датчике
    if (processID < m_ui->t_sensors->rowCount()) {
        QLabel *l_pt_units = (QLabel*) m_ui->t_sensors->cellWidget(processID, 1);
        l_pt_units->setText(units);
    }
}


void NodeTypeParamsPage::processNameChanged(quint8 processID, QString name)
{
    // изменены единицы измерений процесса, надо изменить их на соответствующем датчике
    if (processID < m_ui->t_sensors->rowCount()) {
        QCheckBox* cb_pt_name = (QCheckBox*) m_ui->t_sensors->cellWidget(processID, 0);
        cb_pt_name->setText(name);
    }
}

int NodeTypeParamsPage::pt_findRow(QString pt_name)
{
    // получение номера строки по значению первого стоблца (имя датчика)

    // определяем общее количество узлов
    int rowCount = m_ui->t_sensors->rowCount();

    QCheckBox *cb_pt_name;

    // для всех элементов таблицы
    for (int i = 0; i < rowCount; i++) {
        // получаем CheckBox с именем датчика 
        cb_pt_name = (QCheckBox*) m_ui->t_sensors->cellWidget(i, 0);
        // если имя совпадает с искомым
        if (cb_pt_name->text() == pt_name)
            // возвращаем его
            return i;
    }
    
    // если ничего не нашли - возвращаем -1
    return -1;
}

void NodeTypeParamsPage::createTimerDependence()
{
    // надо добавить в таблицу зависимостей combobox, и 3 кнопки
    // если больше зависимостей нет - надо заблокировать кнопку

    QString depName = m_ui->cb_depends->currentText();

    // удаляем зависимость из списка доступных
    removeTimerDependence(depName);
    
    // создаем элемент таблицы  с названием зависимости
    QTableWidgetItem* ti_depend = new QTableWidgetItem(depName);

    // теперь кнопки
    QToolButton* tb_remove = new QToolButton();
    QToolButton* tb_down = new QToolButton();
    QToolButton* tb_up = new QToolButton();

    tb_remove->setAutoRaise(true);
    tb_down->setAutoRaise(true);
    tb_up->setAutoRaise(true);

    // TODO: сделать так
    // m_toolButton->setPaletteBackgroundColor( m_toolButton->parentWidget()->paletteBackgroundColor());
    
    tb_remove->setText("X");
    tb_down->setText("V");
    tb_up->setText("^");
    
    // размещаем их в таблице

    // создаем строку
    int row = m_ui->t_timerDepends->rowCount();
    m_ui->t_timerDepends->insertRow(row);
    // добавляем в нее элементы
    m_ui->t_timerDepends->setItem(row, 0, ti_depend);
    m_ui->t_timerDepends->setCellWidget(row, 1, tb_remove);
    m_ui->t_timerDepends->setCellWidget(row, 2, tb_down);
    m_ui->t_timerDepends->setCellWidget(row, 3, tb_up);

    // сигналы новых кнопок
    // удалить
    connect(tb_remove, SIGNAL(clicked()),
            this, SLOT(deleteTimerDependence()));
    // вниз
    connect(tb_down, SIGNAL(clicked()),
            this, SLOT(moveDownTimerDependence()));
    // вверх
    connect(tb_up, SIGNAL(clicked()),
            this, SLOT(moveUpTimerDependence()));
}

void NodeTypeParamsPage::deleteTimerDependence()
{
    // FIXME: !!!!!!!!! надо отслеживать координаты нажатой кнопки !!!!!
    // получаем строку на которой нажата кнопка
    int row = m_ui->t_timerDepends->currentRow();
    // получаем имя удаляемой зависимости
    QString name = m_ui->t_timerDepends->item(row, 0)->text();
    // удаляем строку
    m_ui->t_timerDepends->removeRow(row);
    // добавляем зависимость в список
    addTimerDependence(name);
}

void NodeTypeParamsPage::moveDownTimerDependence()
{
    // FIXME: !!!!!!!!! надо отслеживать координаты нажатой кнопки !!!!!
    int row = m_ui->t_timerDepends->currentRow();
    
    QTableWidgetItem* currentItem = m_ui->t_timerDepends->item(row, 0);
    QTableWidgetItem* downItem = m_ui->t_timerDepends->item(row + 1, 0);
    
    QString buffer = downItem->text();
    downItem->setText(currentItem->text());
    currentItem->setText(buffer);
}

void NodeTypeParamsPage::moveUpTimerDependence()
{
    // FIXME: !!!!!!!!! надо отслеживать координаты нажатой кнопки !!!!!
    int row = m_ui->t_timerDepends->currentRow();
    
    QTableWidgetItem* currentItem = m_ui->t_timerDepends->item(row, 0);
    QTableWidgetItem* upItem = m_ui->t_timerDepends->item(row - 1, 0);
    
    QString buffer = upItem->text();
    upItem->setText(currentItem->text());
    currentItem->setText(buffer);
}

void NodeTypeParamsPage::chooseLuaProgrammFile()
{
    // выбор файла
    
    // устанавливаем фильтр на файлы lua
    // получаем имя файла
    QString text = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                "", "Lua Script files (*.lua)");
    
    // записываем имя в интерфейс
    m_ui->le_luaFile->setText(text);
}

void NodeTypeParamsPage::openLuaProgrammFile()
{
    // открытие файла на редактирование во внешнем редакторе

    QDesktopServices::openUrl(m_ui->le_luaFile->text());
}

void NodeTypeParamsPage::chooseLuaStartTimeFile()
{
    // выбор файла
    
    // получаем имя файла
    QString text = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                "", "Lua Script files (*.lua)");

    // записываем имя в интерфейс
    // m_ui->le_luaFileStartTime->setText(text);
}

void NodeTypeParamsPage::openLuaStartTimeFile()
{
    // открытие файла на редактирование во внешнем редакторе
    // QDesktopServices::openUrl(m_ui->le_luaFileStartTime->text());
}

void NodeTypeParamsPage::toggleFunctionLabel(QString label, int tableIndex)
{
    QTableWidget* table;
    if (tableIndex == 0)
        table = m_ui->t_notifyFunctions;
    else
        table = m_ui->t_controlFunctions;

    // FIXME: ну и бред
    QList<QTableWidgetItem *> items = table->findItems(label, Qt::MatchFixedString);
    // если такая функция уже есть
    for (int i = 0; i < items.size(); i++) {
        int itemColumn = table->column(items[i]);
        if (itemColumn == 0) {
            // удаляем ее
            table->removeRow(table->row(items[i]));
            // и выходим
            return;
        }
    }
    
    // если нет - добавляем
    addFunctionLabel(label, tableIndex);
}

void NodeTypeParamsPage::addFunctionLabel(QString label, int tableIndex)
{
    QTableWidget* table;
    if (tableIndex == 0)
        table = m_ui->t_notifyFunctions;
    else
        table = m_ui->t_controlFunctions;

    int rows = table->rowCount();
    QTableWidgetItem *ti_functionLabel = new QTableWidgetItem(label);
    ti_functionLabel->setFlags(Qt::ItemIsEnabled);
    QTableWidgetItem *ti_functionName = new QTableWidgetItem();
    table->insertRow(rows);
    table->setItem(rows, 0, ti_functionLabel);
    table->setItem(rows, 1, ti_functionName);
}

void NodeTypeParamsPage::toggleDirectionAntennaFunction()
{
    bool enabled = m_ui->l_directionalAntennaScript->isEnabled();
    m_ui->l_directionalAntennaScript->setEnabled(!enabled);
    m_ui->le_directionalAntennaScript->setEnabled(!enabled);
    // toggleFunctionLabel(NODE_DIRECTIONAL_ANTENNA);
}

void NodeTypeParamsPage::toggleRealTimerFunction()
{
   bool enabled = m_ui->l_realTimerScript->isEnabled();
   m_ui->l_realTimerScript->setEnabled(!enabled);
   m_ui->le_realTimerScript->setEnabled(!enabled);
 
   // toggleFunctionLabel(NODE_REAL_TIMER);
}

void NodeTypeParamsPage::toggleMovementFunction()
{
   bool enabled = m_ui->l_movingScript->isEnabled();
   m_ui->l_movingScript->setEnabled(!enabled);
   m_ui->le_movingScript->setEnabled(!enabled);
   // toggleFunctionLabel(NODE_MOVEMENT);
}

// --------------------------------------------
nodeTypeParams* NodeTypeParamsPage::getParams()
{
    nodeTypeParams* params = new nodeTypeParams();
    QVector<sensorParams*> sensors;

    // получаем их количество
    int rowCount = m_ui->t_sensors->rowCount();

    // для всех строк таблицы
    for (int i = 0; i < rowCount; i++) {
        sensorParams* sensor = new sensorParams();
        // вытаскиваем из строки таблицы данные о датчике
        QCheckBox *cb_pt_name = (QCheckBox*) m_ui->t_sensors->cellWidget(i, 0);

        // если датчик активирован - записываем его параметры в структуру
        if (cb_pt_name->checkState() == Qt::Checked) {
            QLabel *l_units = (QLabel*) m_ui->t_sensors->cellWidget(i, 1);
            MyDoubleSpinBox *msb_min = (MyDoubleSpinBox*) m_ui->t_sensors->cellWidget(i, 2);
            MyDoubleSpinBox *msb_max = (MyDoubleSpinBox*) m_ui->t_sensors->cellWidget(i, 3);
            // ID датчика на узле
            sensor->sensorID = i;
            // ID измеряемого процесса
            sensor->processID = i;
            // название дачика
            sensor->name = cb_pt_name->text();
            // единицы измерений
            sensor->units = l_units->text();
            // нижняя и верхняя граница чувствительности
            sensor->minValue = msb_min->value();
            sensor->maxValue = msb_max->value();
            
            // добавляем датчик в массив
            sensors.append(sensor);
        }
    }
        
    // ID типа узла
    // params->nodeTypeID = m_ID;
    // имя типа узлов
    params->name = m_name;
    // мощность передатчика
    params->transmitterPower = m_ui->dsb_transmitterPower->value();
    // чувствительность приемника [дБмВт]
    params->receiverSensivity = m_ui->dsb_receiverSensivity->value();
    // диапазон частот [МГц]
    params->frequency = freqBand(m_ui->cb_freqBand->currentIndex());
    // на узлах установлены направленные антенны?
    params->directionalAntenna = m_ui->cb_directionalAntenna->isChecked();
    // массив датчиков
    params->sensors = sensors;
    // невозобновляемый источник питания?
    params->unrenewableEnergy = m_ui->cb_nonrenewable->isChecked();
    // заряд [мАч]
    params->energy = m_ui->sb_energy->value();
    // может ли узел перемещаться?
    params->movement = m_ui->cb_movement->isChecked();
    // использует ли узел реальную модель таймера?
    params->realClock = m_ui->cb_realTimer->isChecked();

    // упорядоченный список ID процессов, от которых зависит таймер
    for (int i = 0; i < m_ui->t_timerDepends->rowCount(); i++) {
        // получаем имя зависимости
        QString name = m_ui->t_timerDepends->item(i, 0)->text();
        // получаем ID из имени
        quint16 procID = pt_findRow(name);
        // добавляем ID в список
        params->clockProcessDependence.append(procID);
    }
    
    // Программа    
    params->luaFile = m_ui->le_luaFile->text();
    // params->luaFileStartTime = m_ui->le_luaFileStartTime->text();

    for (int i = 0; i < m_ui->t_notifyFunctions->rowCount(); i++) {
        QString funcLabel = m_ui->t_notifyFunctions->item(i, 0)->text();
        QString funcName = m_ui->t_notifyFunctions->item(i, 1)->text();
        if (funcName != "")
            params->functions[funcLabel] = funcName;
    }

    for (int i = 0; i < m_ui->t_controlFunctions->rowCount(); i++) {
        QString funcLabel = m_ui->t_controlFunctions->item(i, 0)->text();
        QString funcName = m_ui->t_controlFunctions->item(i, 1)->text();
        if (funcName != "")
            params->functions[funcLabel] = funcName;
    }

    // чтение параметров типов узлов
    return params;
}

void NodeTypeParamsPage::setParams(nodeTypeParams* params)
{
    // ID типа узла
    m_ID = params->nodeTypeID;
    // имя типа узлов
    m_name = params->name;
    // мощность передатчика
    m_ui->dsb_transmitterPower->setValue(params->transmitterPower);
    // чувствительность приемника [дБмВт]
    m_ui->dsb_receiverSensivity->setValue(params->receiverSensivity);
    // диапазон частот [МГц]
    m_ui->cb_freqBand->setCurrentIndex(params->frequency);
    // на узлах установлены направленные антенны?

    // щелкаем на кнопку, если в структуре опция включена, а у нас выключена
    // или если структуре опция выключена, а у нас включена
    if ((params->directionalAntenna && m_ui->cb_directionalAntenna->checkState() == Qt::Unchecked)
        || (!params->directionalAntenna && m_ui->cb_directionalAntenna->checkState() == Qt::Checked))
        m_ui->cb_directionalAntenna->click();

    // массив датчиков
    for (int i = 0; i < params->sensors.size(); i++) {
        if (params->sensors[i] == NULL)
            break;
        // получаем параметры очередного датчика
        sensorParams* sensor = params->sensors[i];
        
        // ищем датчик в таблице по его ID
        QCheckBox *cb_pt_name = (QCheckBox*) m_ui->t_sensors->cellWidget(sensor->sensorID, 0);
        // активируем его
        cb_pt_name->click();
        
        // получаем SpinBox'ы диапазона чувствительности
        MyDoubleSpinBox *msb_min = (MyDoubleSpinBox*) m_ui->t_sensors->cellWidget(sensor->sensorID, 2);
        MyDoubleSpinBox *msb_max = (MyDoubleSpinBox*) m_ui->t_sensors->cellWidget(sensor->sensorID, 3);
        
        // изменяем его диапазон чувствительности
        msb_min->setValue(sensor->minValue);
        msb_max->setValue(sensor->maxValue);
        
    }
    
    // невозобновляемый источник питания?
    m_ui->cb_nonrenewable->setCheckState(Qt::CheckState(params->unrenewableEnergy ? Qt::Checked : Qt::Unchecked));
    // заряд [мАч]
    m_ui->sb_energy->setValue(params->energy);
    // может ли узел перемещаться?

    // щелкаем на кнопку, если в структуре опция включена, а у нас выключена
    // или если структуре опция выключена, а у нас включена
    if ((params->movement && m_ui->cb_movement->checkState() == Qt::Unchecked)
        || (!params->movement && m_ui->cb_movement->checkState() == Qt::Checked))
        m_ui->cb_movement->click();
    
    // использует ли узел реальную модель таймера?
    // щелкаем на кнопку, если в структуре опция включена, а у нас выключена
    // или если структуре опция выключена, а у нас включена
    if ((params->realClock && m_ui->cb_realTimer->checkState() == Qt::Unchecked)
        || (!params->realClock && m_ui->cb_realTimer->checkState() == Qt::Checked))
        m_ui->cb_realTimer->click();

    // программа    
    m_ui->le_luaFile->clear();

    m_ui->le_luaFile->setText(params->luaFile);

    // m_ui->le_luaFileStartTime->setText(params->luaFileStartTime);
}
