#include "BuildWidget.h"

#include <QToolBar>
#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QFileDialog>

#include "ProjectModel.h"
#include "Settings.h"

BuildWidget::BuildWidget(QWidget *parent) : QWidget(parent)
{
    initUi();
    changeProject(Settings::instance()->value(SETTINGS_BUILD_PATH).toString());
}

BuildWidget::~BuildWidget()
{
    Settings::instance()->setValue(SETTINGS_BUILD_PATH, currentPath_label->text());
}

void BuildWidget::initUi()
{
    {
        actions_toolBar = new QToolBar(this);
        auto act = new QAction(actions_toolBar);
        act->setToolTip("Указать путь к проекту");
        act->setIcon(QIcon(":/buildWidget/ico/suitcase.svg"));
        connect(act, &QAction::triggered, this, &BuildWidget::slot_changeProject);
        actions_toolBar->addAction(act);
    }

    currentPath_label = new QLabel(this);
    QFont boldFont;
    boldFont.setBold(true);
    currentPath_label->setFont(boldFont);
    currentPath_label->setIndent(5);
    currentPath_label->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);

    project_treeView = new QTreeView(this);
    project_model = new ProjectModel(this);
    project_treeView->setModel(project_model);
    for (int i = 1; i < project_model->columnCount(); ++i)
    {
        project_treeView->hideColumn(i);
    }

    auto main_vLay = new QVBoxLayout();
    main_vLay->setContentsMargins(0, 0, 0, 0);
    main_vLay->setSpacing(0);
    main_vLay->addWidget(actions_toolBar);
    main_vLay->addWidget(currentPath_label);
    main_vLay->addSpacing(3);
    main_vLay->addWidget(project_treeView);
    setLayout(main_vLay);
}

void BuildWidget::changeProject(const QString &path)
{
    if (path.isEmpty())
    {
        return;
    }
    currentPath_label->setText(path);
    project_model->setRootPath(path);
    project_treeView->setRootIndex(project_model->index(path));
}

void BuildWidget::slot_changeProject()
{
    auto currentPath = currentPath_label->text();
    if (currentPath.isEmpty())
    {
        currentPath = QDir::homePath();
    }
    const auto folderPath = QFileDialog::getExistingDirectory(this, QStringLiteral("Укажите путь к проекту"), currentPath);
    if (folderPath.isEmpty())
    {
        return;
    }
    changeProject(folderPath);
}
