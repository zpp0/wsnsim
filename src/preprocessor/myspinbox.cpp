#include "myspinbox.h"

MyDoubleSpinBox::MyDoubleSpinBox ()
{}

MyDoubleSpinBox::~MyDoubleSpinBox ()
{}

void MyDoubleSpinBox::setMax (double value)
{
    setMaximum (value);
}


void MyDoubleSpinBox::setMin (double value)
{
    setMinimum (value);
}
