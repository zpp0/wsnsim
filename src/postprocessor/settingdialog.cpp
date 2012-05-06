#include <QtGui>
#include "settingdialog.h"
#include "filter.h"

SettingDialog::SettingDialog(Filter *flt, QWidget *parent)
    : QDialog(parent)
{
    QVector<quint8> filter =  flt->getFilter();
    //текущий флажок
    QCheckBox *pchk = 0;
    //список событий за которые отвечают флажки
    QStringList lstNames;

    //создаем список событий
    lstNames << tr("Узел принял сообщение по радио") << tr("Прием узлом сообщения по радио")
            << tr("Передача узлом сообщения по радио") << tr("Проверка узлом локального таймера")
            << tr("Измерение") << tr("Перемещение узла")
            << tr("Запись в журнал локального времени узла") << tr("Изменение связи между узлами");

    //менеджер компоновки флажков
    QVBoxLayout* pChecksLayout = new QVBoxLayout;
    QStringList::iterator i;
    int j = 0;
    for (i = lstNames.begin(); i != lstNames.end(); i++)
    {
        //создаем новый флажок
        pchk = new QCheckBox(*i);
        //в зависимости от настроек включаем флажок
        if ( filter[j] == 1)
        {
            pchk->setChecked(true);
        }
        else
        {
            pchk->setChecked(false);
        }
        //сохраняем указатель на флажок
        m_checks.append(pchk);
        //отображаем флажок на экране
        pChecksLayout->addWidget(pchk);
        j++;
    }

    //создаем кнопки применения и отмены действий
    QDialogButtonBox * pbuttonBox = new QDialogButtonBox;
    pbuttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    //нажатие на кнопку ok вызовет посылку изменения фильтра событий
    connect(pbuttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(slotOkButtonClicked()));
    //кнопка cancel закрывает окно
    connect(pbuttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *pmainLayout = new QVBoxLayout;
    pmainLayout->addLayout(pChecksLayout);
    pmainLayout->addWidget(pbuttonBox);

    setLayout(pmainLayout);
}

SettingDialog::~SettingDialog()
{
    //удаляем флажки
    for (int i = 0; i < m_checks.size(); i++)
    {
        delete m_checks.at(i);
    }
    m_checks.clear();
    //остальные объекты уничтожатся сами при закрытии окна
}

void SettingDialog::slotOkButtonClicked()
{
    QVector<quint8> newfilter;
    for (int i = 0; i < m_checks.size(); i++)
    {
        if ( m_checks[i]->isChecked() )
        {
            //отображать событие нажат
            newfilter.append(1);
        }
        else
        {
            //не отображать событие
            newfilter.append(0);
        }
    }
    //делаем диалог невидимым для пользователя пока строится индекс
    setVisible(false);
    //посылаем сигнал изменения фильтра событий
    emit signalFilterChanged(newfilter);
    //уничтожаем окно
    close();
}
