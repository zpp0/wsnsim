#include <QDoubleSpinBox>

class MyDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    
    MyDoubleSpinBox ();
    virtual ~MyDoubleSpinBox ();

public slots:
    void setMax (double value);
    void setMin (double value);
};
