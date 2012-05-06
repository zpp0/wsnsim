#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QtGui/QDialog>
#include <QVector>

class QCheckBox;
class Filter;

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    SettingDialog(Filter *filter, QWidget *parent = 0);
    ~SettingDialog();

signals:
    void signalFilterChanged(QVector<quint8>);

public slots:
    void slotOkButtonClicked();

private:
    QVector<QCheckBox*> m_checks;
};

#endif // SETTINGDIALOG_H
