#include "projectPage.h"
#include "ui_projectPage.h"

#include "dataParams.h"

ProjectPage::ProjectPage () :
    m_ui(new Ui::ProjectPage)
{
    m_ui->setupUi (this);
}

ProjectPage::~ProjectPage ()
{
    delete m_ui;
}

projectParams* ProjectPage::getParams()
{
    // выделяем память под структуру
    projectParams* params = new projectParams();

    // записываем параметры
    // автор проекта
    params->autor = m_ui->le_autorProject->text();
    // название проекта
    params->projectName = m_ui->le_nameProject->text();
    // комментарий
    params->comment = m_ui->te_commentProject->toPlainText();

    // возвращаем результат
    return params;
}

void ProjectPage::setParams(projectParams* params)
{
    // устанавливаем параметры
    if (params != NULL) {
        if (params->autor != NULL)
            // автор проекта
            m_ui->le_autorProject->setText(params->autor);
        if (params->projectName != NULL)
            // название проекта
            m_ui->le_nameProject->setText(params->projectName);
        if (params->comment != NULL)
            // комментарий
            m_ui->te_commentProject->setPlainText(params->comment);
    }
}
