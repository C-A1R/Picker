#include "BuildWidget.h"

#include <QToolBar>
#include <QActionGroup>
#include <QLabel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

#include "ProjectTreeView.h"
#include "ProjectProxyModel.h"
#include "Settings.h"

BuildWidget::BuildWidget(QWidget *parent)
    : QWidget(parent),
    saveOptions{SaveOpt::fromInt(Settings::instance()->value(SETTINGS_SAVE_OPTIONS, SaveOptions::SAVE_TO_FOLDERS).toInt())}
{
    initUi();
    changeProject(Settings::instance()->value(SETTINGS_BUILD_PATH).toString());
    connect(project_treeView, &ProjectTreeView::signal_dropped, proxy_model, &ProjectProxyModel::slot_dropped);
    connect(proxy_model, &ProjectProxyModel::signal_dropped, project_model, &model_type::slot_dropped);
    connect(project_treeView, &ProjectTreeView::signal_setChecked, proxy_model, &ProjectProxyModel::slot_setChecked);
    connect(proxy_model, &ProjectProxyModel::signal_setChecked, project_model, &model_type::slot_setChecked);
}

BuildWidget::~BuildWidget()
{
    Settings::instance()->setValue(SETTINGS_BUILD_PATH, currentPath_label->text());
    Settings::instance()->setValue(SETTINGS_SAVE_OPTIONS, saveOptions.toInt());
}

void BuildWidget::initUi()
{
    actions_toolBar = new QToolBar(this);
    {
        auto act = new QAction(actions_toolBar);
        act->setToolTip("Указать путь к проекту");
        act->setIcon(QIcon(":/buildWidget/ico/suitcase.svg"));
        connect(act, &QAction::triggered, this, &BuildWidget::slot_changeProject);
        actions_toolBar->addAction(act);
    }
    {
        auto act = new QAction(actions_toolBar);
        act->setToolTip("Сохранить список");
        act->setIcon(QIcon(":/buildWidget/ico/save.svg"));
        connect(act, &QAction::triggered, this, &BuildWidget::slot_saveList);
        actions_toolBar->addAction(act);
    }
    {
        auto act = new QAction(actions_toolBar);
        act->setToolTip("Собрать");
        act->setIcon(QIcon(":/buildWidget/ico/build.svg"));
        connect(act, &QAction::triggered, this, &BuildWidget::slot_build);
        actions_toolBar->addAction(act);
    }

    saveOptions_toolBar = new QToolBar(this);
    {
        auto act = new QAction(saveOptions_toolBar);
        act->setToolTip("Сохранить в каталогах");
        act->setIcon(QIcon(":/buildWidget/ico/folders.svg"));
        act->setCheckable(true);
        act->setChecked(saveOptions.testFlag(SaveOptions::SAVE_TO_FOLDERS));
        connect(act, &QAction::triggered, this, &BuildWidget::slot_saveToFoldersOptionChanged);
        saveOptions_toolBar->addAction(act);
    }
    {
        auto act = new QAction(saveOptions_toolBar);
        act->setToolTip("Сохранить в указанный каталог");
        act->setIcon(QIcon(":/buildWidget/ico/folder.svg"));
        act->setCheckable(true);
        act->setChecked(saveOptions.testFlag(SaveOptions::SAVE_TO_DEFENIT_FOLDER));
        connect(act, &QAction::triggered, this, &BuildWidget::slot_saveToDefenitFolderOptionChanged);
        saveOptions_toolBar->addAction(act);
    }

    auto tools_hLay = new QHBoxLayout();
    tools_hLay->addWidget(actions_toolBar);
    tools_hLay->addStretch();
    tools_hLay->addWidget(saveOptions_toolBar);

    currentPath_label = new QLabel(this);
    QFont boldFont;
    boldFont.setBold(true);
    currentPath_label->setFont(boldFont);
    currentPath_label->setIndent(5);
    currentPath_label->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);

    project_treeView = new ProjectTreeView(this);
    project_treeView->header()->hide();
    project_model = new model_type(this);
    project_model->setReadOnly(true);
    proxy_model = new ProjectProxyModel();
    proxy_model->setSourceModel(project_model);
    proxy_model->setDynamicSortFilter(false);
    project_treeView->setModel(proxy_model);
    project_treeView->setSortingEnabled(true);
    project_treeView->sortByColumn(0, Qt::AscendingOrder);
    project_treeView->setDragDropMode(QAbstractItemView::InternalMove);
    project_treeView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    project_treeView->setDragEnabled(true);
    project_treeView->setAcceptDrops(true);
    project_treeView->setDropIndicatorShown(true);

    for (int i = 1; i < project_model->columnCount(); ++i)
    {
        project_treeView->hideColumn(i);
    }

    auto main_vLay = new QVBoxLayout();
    main_vLay->setContentsMargins(0, 0, 0, 0);
    main_vLay->setSpacing(0);
    main_vLay->addLayout(tools_hLay);
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
    project_treeView->setRootIndex(proxy_model->mapFromSource(project_model->index(path)));
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

void BuildWidget::slot_saveList()
{

}

void BuildWidget::slot_build()
{

}

void BuildWidget::slot_saveToFoldersOptionChanged(bool checked)
{
    saveOptions.setFlag(SaveOptions::SAVE_TO_FOLDERS, checked);
}

void BuildWidget::slot_saveToDefenitFolderOptionChanged(bool checked)
{
    saveOptions.setFlag(SaveOptions::SAVE_TO_DEFENIT_FOLDER, checked);
}
