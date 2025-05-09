#include "BuildWidget.h"
#include "ProjectTreeView.h"
#include "ProjectProxyModel.h"
#include "Settings.h"
#include "SqlMgr.h"
#include "PdfBuilder/ToProjectDirectoriesPdfBuilder.h"
#include "PdfBuilder/ToSeparateDirectoryPdfBuilder.h"
#include "PdfBuilder/ToProjectAndSeparateDirectoriesPdfBuilder.h"

#include <QToolBar>
#include <QActionGroup>
#include <QLabel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSharedPointer>


BuildWidget::BuildWidget(QWidget *parent)
    : QWidget(parent)
    , saveOptions{SaveOpt::fromInt(Settings::instance()->value(SETTINGS_SAVE_OPTIONS, SaveOptions::SAVE_TO_PROJECT_DIRECTORIES).toInt())}
{
    initUi();
    {
        // connect(project_model, &ProjectFileSystemModel::signal_expand, proxy_model, &ProjectProxyModel::slot_expand);
        // connect(proxy_model, &ProjectProxyModel::signal_expand, project_treeView, &ProjectTreeView::slot_expand);
    }
    {
        // connect(project_treeView, &ProjectTreeView::signal_dropped, proxy_model, &ProjectProxyModel::slot_dropped);
        // connect(proxy_model, &ProjectProxyModel::signal_dropped, project_model, &ProjectFileSystemModel::slot_dropped);
    }
    {
        // connect(project_treeView, &ProjectTreeView::signal_added, proxy_model, &ProjectProxyModel::slot_added);
        // connect(proxy_model, &ProjectProxyModel::signal_added, project_model, &ProjectFileSystemModel::slot_added);
    }
    {
        // connect(project_treeView, &ProjectTreeView::signal_setChecked, proxy_model, &ProjectProxyModel::slot_setChecked);
        // connect(proxy_model, &ProjectProxyModel::signal_setChecked, project_model, &ProjectFileSystemModel::slot_setChecked);
        connect(project_treeView, &ProjectTreeView::signal_setChecked, project_model, &ProjectModel::slot_setChecked);
    }
    changeProject(Settings::instance()->value(SETTINGS_BUILD_PATH).toString());
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
        act->setChecked(saveOptions.testFlag(SaveOptions::SAVE_TO_PROJECT_DIRECTORIES));
        connect(act, &QAction::triggered, this, &BuildWidget::slot_saveToFoldersOptionChanged);
        saveOptions_toolBar->addAction(act);
    }
    {
        auto act = new QAction(saveOptions_toolBar);
        act->setToolTip("Сохранить в указанный каталог");
        act->setIcon(QIcon(":/buildWidget/ico/folder.svg"));
        act->setCheckable(true);
        act->setChecked(saveOptions.testFlag(SaveOptions::SAVE_TO_SEPARATE_DIRECTORY));
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
    project_model = new ProjectModel(this);
    // project_model->setReadOnly(true);
    // proxy_model = new ProjectProxyModel();
    // proxy_model->setSourceModel(project_model);
    // proxy_model->setDynamicSortFilter(false);
    project_treeView->setModel(project_model);
    project_treeView->setSortingEnabled(true);
    project_treeView->sortByColumn(ProjectFileSystemModel::col_Name, Qt::AscendingOrder);
    project_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    project_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    project_treeView->setDragEnabled(true);
    project_treeView->viewport()->setAcceptDrops(true);
    project_treeView->setDropIndicatorShown(true);

    project_treeView->hideColumn(ProjectFileSystemModel::Columns::col_Size);
    project_treeView->hideColumn(ProjectFileSystemModel::Columns::col_Type);
    project_treeView->hideColumn(ProjectFileSystemModel::Columns::col_DateModified);
    project_treeView->header()->setSectionResizeMode(ProjectFileSystemModel::Columns::col_Name, QHeaderView::Stretch);
    project_treeView->header()->setSectionResizeMode(ProjectFileSystemModel::Columns::col_ResultHolder, QHeaderView::Fixed);
    project_treeView->header()->setStretchLastSection(false);
    project_treeView->header()->resizeSection(ProjectFileSystemModel::Columns::col_ResultHolder, 0);

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
        return;
    if (!project_model->setProjectPath(path))
        return;
    currentPath_label->setText(path);
}

QString BuildWidget::getDefenitFolder() const
{
    QString defenitFolder{Settings::instance()->value(SETTINGS_DEFENIT_PATH).toString()};
    auto currentPath = defenitFolder;
    if (currentPath.isEmpty())
    {
        currentPath = QDir::homePath();
    }
    defenitFolder = QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Укажите путь для сохранения файлов"), currentPath);
    if (defenitFolder.isEmpty())
    {
        return defenitFolder;
    }
    if (!defenitFolder.endsWith('/'))
    {
        defenitFolder += '/';
    }
    Settings::instance()->setValue(SETTINGS_DEFENIT_PATH, defenitFolder);
    return defenitFolder;
}

/// работает с индексами прокси-модели
void BuildWidget::saveTree(const QModelIndex &rootIndex, SqlMgr &sqlMgr) const
{
    // if (!rootIndex.isValid())
    // {
    //     return;
    // }
    // const int rows = proxy_model->rowCount(rootIndex);
    // if (!rows)
    // {
    //     return;
    // }

    // for (int i = 0; i < rows; ++i)
    // {
    //     const QModelIndex &childIndex = proxy_model->index(i, ProjectFileSystemModel::Columns::col_Name, rootIndex);
    //     const QModelIndex &sourceChildIndex = proxy_model->mapToSource(childIndex);
    //     const QModelIndex &sourceChildIndex_resultHolderCol = sourceChildIndex.siblingAtColumn(ProjectFileSystemModel::Columns::col_ResultHolder);
    //     const QFileInfo &info = project_model->fileInfo(sourceChildIndex);
    //     if (!sqlMgr.insertProjectElement(project_model->data(sourceChildIndex, Qt::CheckStateRole).value<Qt::CheckState>(),
    //                                      project_model->data(sourceChildIndex_resultHolderCol, Qt::CheckStateRole).value<Qt::CheckState>(),
    //                                      project_treeView->isExpanded(childIndex),
    //                                      info.absoluteFilePath()))
    //     {
    //         qDebug() << "insertion failed: " << info.absoluteFilePath();
    //     }
    //     saveTree(childIndex, sqlMgr);
    // }
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
    // const QString dbFilename = project_model->listFilePath();
    // if (QFile::exists(dbFilename))
    // {
    //     QFile::remove(dbFilename);
    // }

    // SqlMgr sqlMgr(dbFilename);
    // if (!sqlMgr.open())
    // {
    //     qDebug("can`t open db");
    //     return;
    // }
    // if (!sqlMgr.createPickerDb())
    // {
    //     qDebug("can`t create db");
    //     return;
    // }

    // if (!sqlMgr.transaction())
    // {
    //     qDebug("can`t start transaction");
    //     return;
    // }
    // saveTree(proxy_model->mapFromSource(project_model->index(project_model->rootPath())), sqlMgr);
    // if (!sqlMgr.commit())
    // {
    //     qDebug("can`t commit transaction");
    //     return;
    // }
}

void BuildWidget::slot_build()
{
    // if (saveOptions == SaveOptions::SAVE_NONE)
    // {
    //     QMessageBox::warning(this, windowTitle(), "Не выбраны опции сохранения");
    //     return;
    // }
    // const auto checkedPdf = project_model->getCheckedPdfPaths();
    // if (checkedPdf.isEmpty())
    // {
    //     QMessageBox::warning(this, windowTitle(), "Не выбраны файлы для сохранения");
    //     return;
    // }

    // if (saveOptions == SaveOptions::SAVE_TO_PROJECT_DIRECTORIES)
    // {
    //     builder.reset(new ToProjectDirectoriesPdfBuilder(project_model->getResultHolders()));
    // }
    // else if (saveOptions == SaveOptions::SAVE_TO_SEPARATE_DIRECTORY)
    // {
    //     QString defenitFolder = getDefenitFolder();
    //     if (defenitFolder.isEmpty())
    //     {
    //         return;
    //     }
    //     builder.reset(new ToSeparateDirectoryPdfBuilder(project_model->getResultHolders(), std::move(defenitFolder)));
    // }
    // else if (saveOptions.testFlag(SaveOptions::SAVE_TO_PROJECT_DIRECTORIES)
    //            && saveOptions.testFlag(SaveOptions::SAVE_TO_SEPARATE_DIRECTORY))
    // {
    //     QString defenitFolder = getDefenitFolder();
    //     if (defenitFolder.isEmpty())
    //     {
    //         return;
    //     }
    //     builder.reset(new ToProjectAndSeparateDirectoryPdfBuilder(project_model->getResultHolders(),  std::move(defenitFolder)));
    // }
    // else
    // {
    //     builder.reset(nullptr);
    // }
    // if (builder.isNull())
    // {
    //     QMessageBox::critical(this, windowTitle(), "Не могу выполнить сборку");
    //     return;
    // }
    // connect(builder.get(), &IPdfBuilder::signal_finished, this, &BuildWidget::slot_buildFinished);
    // connect(builder.get(), &IPdfBuilder::signal_cancelled, this, &BuildWidget::slot_buildCancelled);
    // builder->exec(checkedPdf);
}

void BuildWidget::slot_saveToFoldersOptionChanged(bool checked)
{
    saveOptions.setFlag(SaveOptions::SAVE_TO_PROJECT_DIRECTORIES, checked);
}

void BuildWidget::slot_saveToDefenitFolderOptionChanged(bool checked)
{
    saveOptions.setFlag(SaveOptions::SAVE_TO_SEPARATE_DIRECTORY, checked);
}

void BuildWidget::slot_buildFinished()
{
    QMessageBox::information(this, windowTitle(), "Сборка завершена");
}

void BuildWidget::slot_buildCancelled()
{
    QMessageBox::information(this, windowTitle(), "Сборка отменена пользователем");
}
