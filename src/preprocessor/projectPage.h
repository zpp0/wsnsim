#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H

#include <QGroupBox>
#include "dataParams.h"

namespace Ui {
    class ProjectPage;
}

class ProjectPage : public QGroupBox
{
    Q_OBJECT

public:
    ProjectPage();
    virtual ~ProjectPage();

    projectParams* getParams();
    void setParams(projectParams* params);
    
private:    
    Ui::ProjectPage *m_ui;
};

#endif // PROJECTPAGE_H
