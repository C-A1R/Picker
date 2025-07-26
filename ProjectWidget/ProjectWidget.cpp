#include "ProjectWidget.h"
#include "ProjectTreeView.h"
#include "Settings.h"
#include "SqlMgr.h"

#include "Commands/ItemsCheckedCmd.h"
#include "Commands/ResultHolderCheckedCmd.h"

#include "PdfBuilder/ToProjectDirectoriesPdfBuilder.h"
#include "PdfBuilder/ToSeparateDirectoryPdfBuilder.h"
#include "PdfBuilder/ToProjectAndSeparateDirectoriesPdfBuilder.h"

#include <QToolBar>
#include <QLabel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyleHints>
#include <QApplication>
#include <QUndoStack>


ProjectWidget::ProjectWidget(QWidget *parent)
    : QWidget(parent)
    , saveOptions{SaveOpt::fromInt(Settings::instance()->value(SETTINGS_SAVE_OPTIONS, SaveOptions::SAVE_TO_PROJECT_DIRECTORIES).toInt())}
    , undoStack{new QUndoStack(this)}
{
    initUi();

    connect(project_treeView, &ProjectTreeView::signal_itemsChecked,            this,   &ProjectWidget::slot_itemsChecked);
    connect(project_treeView, &ProjectTreeView::signal_resultHolderChecked,     this,   &ProjectWidget::slot_resultHolderChecked);

    connect(project_model,    &ProjectModel::signal_expand,         project_treeView,   &ProjectTreeView::slot_expand);
    connect(project_treeView, &ProjectTreeView::signal_dropped,     project_model,      &ProjectModel::slot_dropped);
    connect(project_treeView, &ProjectTreeView::signal_added,       project_model,      &ProjectModel::slot_added);

    changeProject(Settings::instance()->value(SETTINGS_BUILD_PATH).toString());
}

ProjectWidget::~ProjectWidget()
{
    Settings::instance()->setValue(SETTINGS_BUILD_PATH, currentPath_label->text());
    Settings::instance()->setValue(SETTINGS_SAVE_OPTIONS, saveOptions.toInt());
}

void ProjectWidget::initUi()
{
    const bool isDarkTheme = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    QToolBar *actions_toolBar = new QToolBar(this);
    {
        auto act = new QAction(actions_toolBar);
        act->setToolTip("Открыть проект");
        const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/open_dark.svg")
                                       : QIcon(":/buildWidget/ico/open.svg");
        act->setIcon(icon);
        connect(act, &QAction::triggered, this, &ProjectWidget::slot_changeProject);
        actions_toolBar->addAction(act);
    }
    {
        auto act = new QAction(actions_toolBar);
        act->setToolTip("Сохранить проект");
        const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/save_dark.svg")
                                       : QIcon(":/buildWidget/ico/save.svg");
        act->setIcon(icon);
        connect(act, &QAction::triggered, this, &ProjectWidget::slot_saveProject);
        actions_toolBar->addAction(act);
    }
    {
        auto act = new QAction(actions_toolBar);
        act->setToolTip("Собрать");
        const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/build_dark.svg")
                                       : QIcon(":/buildWidget/ico/build.svg");
        act->setIcon(icon);
        connect(act, &QAction::triggered, this, &ProjectWidget::slot_build);
        actions_toolBar->addAction(act);
    }

    QToolBar *undoRedo_toolBar = new QToolBar(this);
    {
        auto act = new QAction(undoRedo_toolBar);
        act->setToolTip("Отмена");
        const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/undo_dark.svg")
                                       : QIcon(":/buildWidget/ico/undo.svg");
        act->setIcon(icon);
        act->setShortcut(QKeySequence::Undo);
        act->setEnabled(undoStack->canUndo());
        undoRedo_toolBar->addAction(act);
        connect(act, &QAction::triggered, undoStack, &QUndoStack::undo);
        connect(undoStack, &QUndoStack::canUndoChanged, act, &QAction::setEnabled);
    }
    {
        auto act = new QAction(undoRedo_toolBar);
        act->setToolTip("Повтор");
        const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/redo_dark.svg")
                                       : QIcon(":/buildWidget/ico/redo.svg");
        act->setIcon(icon);
        act->setShortcut(QKeySequence::Redo);
        act->setEnabled(undoStack->canRedo());
        undoRedo_toolBar->addAction(act);
        connect(act, &QAction::triggered, undoStack, &QUndoStack::redo);
        connect(undoStack, &QUndoStack::canRedoChanged, act, &QAction::setEnabled);
    }

    QToolBar *saveOptions_toolBar = new QToolBar(this);
    {
        auto act = new QAction(saveOptions_toolBar);
        act->setToolTip("Сохранить в каталогах");
        const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/folders_dark.svg")
                                       : QIcon(":/buildWidget/ico/folders.svg");
        act->setIcon(icon);
        act->setCheckable(true);
        act->setChecked(saveOptions.testFlag(SaveOptions::SAVE_TO_PROJECT_DIRECTORIES));
        connect(act, &QAction::triggered, this, &ProjectWidget::slot_saveToFoldersOptionChanged);
        saveOptions_toolBar->addAction(act);
    }
    {
        auto act = new QAction(saveOptions_toolBar);
        act->setToolTip("Сохранить в указанный каталог");
        const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/folder_dark.svg")
                                       : QIcon(":/buildWidget/ico/folder.svg");
        act->setIcon(icon);
        act->setCheckable(true);
        act->setChecked(saveOptions.testFlag(SaveOptions::SAVE_TO_SEPARATE_DIRECTORY));
        connect(act, &QAction::triggered, this, &ProjectWidget::slot_saveToDefenitFolderOptionChanged);
        saveOptions_toolBar->addAction(act);
    }

    auto tools_hLay = new QHBoxLayout();
    tools_hLay->addWidget(actions_toolBar);
    tools_hLay->addStretch();
    tools_hLay->addWidget(undoRedo_toolBar);
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
    project_treeView->setModel(project_model);
    project_treeView->setSortingEnabled(true);
    project_treeView->sortByColumn(Columns::col_Name, Qt::AscendingOrder);
    project_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    project_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    project_treeView->setDragEnabled(true);
    project_treeView->viewport()->setAcceptDrops(true);
    project_treeView->setDropIndicatorShown(true);
    project_treeView->header()->setSectionResizeMode(Columns::col_Name, QHeaderView::Stretch);
    project_treeView->header()->setSectionResizeMode(Columns::col_LastModified, QHeaderView::ResizeToContents);
    project_treeView->header()->setSectionResizeMode(Columns::col_ResultHolder, QHeaderView::Fixed);
    project_treeView->header()->setStretchLastSection(false);
    project_treeView->header()->resizeSection(Columns::col_ResultHolder, 0);

    auto main_vLay = new QVBoxLayout();
    main_vLay->setContentsMargins(0, 0, 0, 0);
    main_vLay->setSpacing(0);
    main_vLay->addLayout(tools_hLay);
    main_vLay->addWidget(currentPath_label);
    main_vLay->addSpacing(3);
    main_vLay->addWidget(project_treeView);
    setLayout(main_vLay);
}

void ProjectWidget::changeProject(const QString &path)
{
    if (path.isEmpty())
        return;

    if (!project_model->loadProjectItems(path))
        return;
    currentPath_label->setText(path);
    currentPath_label->setToolTip(path);
    project_treeView->expand(project_model->index(0, 0));
}

QString ProjectWidget::getDefenitFolder() const
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

void ProjectWidget::saveProjectTree(SqlMgr &sqlMgr) const
{
    const std::shared_ptr<const ProjectItem> &rootItem = project_model->projectRootItem();
    if (!rootItem)
    {
        return;
    }

    project_model->reorder();//пересчет порядковых индексов, чтобы отбросить дробные части

    const int rows = rootItem->childCount();
    if (!rows)
    {
        return;
    }

    for (int i = 0; i < rows; ++i)
    {
        const QModelIndex &childIndex = project_model->index(i, Columns::col_Name, QModelIndex());
        saveProjectItem(childIndex, sqlMgr);
    }
}

void ProjectWidget::saveProjectItem(const QModelIndex &itemIndex, SqlMgr &sqlMgr) const
{
    if (!itemIndex.isValid())
    {
        return;
    }
    saveItemToDB(itemIndex, sqlMgr);
    const int rows = project_model->rowCount(itemIndex);
    if (!rows)
    {
        return;
    }

    for (int i = 0; i < rows; ++i)
    {
        const QModelIndex &childIndex = project_model->index(i, Columns::col_Name, itemIndex);
        saveProjectItem(childIndex, sqlMgr);
    }
}

void ProjectWidget::saveItemToDB(const QModelIndex &index, SqlMgr &sqlMgr) const
{
    auto item = static_cast<const ProjectItem*>(index.internalPointer());
    if (!item)
    {
        return;
    }
    const std::shared_ptr<const ProjectItem> parentItem = item->parentItem();
    if (!parentItem)
    {
        return;
    }

    const QModelIndex &index_resultHolderCol = index.siblingAtColumn(Columns::col_ResultHolder);
    if (!sqlMgr.insertProjectElement(item->getId()
                                     , parentItem->getId()
                                     , item->getOrderIndex()
                                     , project_model->data(index, Qt::CheckStateRole).value<Qt::CheckState>()
                                     , project_model->data(index_resultHolderCol, Qt::CheckStateRole).value<Qt::CheckState>()
                                     , project_treeView->isExpanded(index)
                                     , item->getPath().absolutePath()))
    {
        qDebug() << "insertion failed: " << item->getPath().absolutePath();
    }
}

void ProjectWidget::slot_changeProject()
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

void ProjectWidget::slot_saveProject()
{
    const QString dbFilename = project_model->projectDbFilePath();
    if (QFile::exists(dbFilename))
    {
        QFile::remove(dbFilename);
    }

    SqlMgr sqlMgr(dbFilename);
    if (!sqlMgr.open())
    {
        qDebug("can`t open db");
        return;
    }
    if (!sqlMgr.createPickerDb())
    {
        qDebug("can`t create db");
        return;
    }

    if (!sqlMgr.transaction())
    {
        qDebug("can`t start transaction");
        return;
    }
    saveProjectTree(sqlMgr);
    if (!sqlMgr.commit())
    {
        qDebug("can`t commit transaction");
        return;
    }
}

void ProjectWidget::slot_build()
{
    if (saveOptions == SaveOptions::SAVE_NONE)
    {
        QMessageBox::warning(this, windowTitle(), "Не выбраны опции сборки");
        return;
    }
    const QHash<QString, QStringList> structure = project_model->makeBuildFileStructure();
    if (structure.isEmpty())
    {
        QMessageBox::warning(this, windowTitle(), "Не выбраны файлы для сборки");
        return;
    }

    if (saveOptions == SaveOptions::SAVE_TO_PROJECT_DIRECTORIES)
    {
        builder.reset(new ToProjectDirectoriesPdfBuilder());
    }
    else if (saveOptions == SaveOptions::SAVE_TO_SEPARATE_DIRECTORY)
    {
        QString defenitFolder = getDefenitFolder();
        if (defenitFolder.isEmpty())
        {
            return;
        }
        builder.reset(new ToSeparateDirectoryPdfBuilder(std::move(defenitFolder)));
    }
    else if (saveOptions.testFlag(SaveOptions::SAVE_TO_PROJECT_DIRECTORIES)
               && saveOptions.testFlag(SaveOptions::SAVE_TO_SEPARATE_DIRECTORY))
    {
        QString defenitFolder = getDefenitFolder();
        if (defenitFolder.isEmpty())
        {
            return;
        }
        builder.reset(new ToProjectAndSeparateDirectoryPdfBuilder(std::move(defenitFolder)));
    }
    else
    {
        builder.reset(nullptr);
    }
    if (builder.isNull())
    {
        QMessageBox::critical(this, windowTitle(), "Не могу выполнить сборку");
        return;
    }
    connect(builder.get(), &IPdfBuilder::signal_finished, this, &ProjectWidget::slot_buildFinished);
    connect(builder.get(), &IPdfBuilder::signal_cancelled, this, &ProjectWidget::slot_buildCancelled);
    builder->exec(structure);
}

void ProjectWidget::slot_saveToFoldersOptionChanged(bool checked)
{
    saveOptions.setFlag(SaveOptions::SAVE_TO_PROJECT_DIRECTORIES, checked);
}

void ProjectWidget::slot_saveToDefenitFolderOptionChanged(bool checked)
{
    saveOptions.setFlag(SaveOptions::SAVE_TO_SEPARATE_DIRECTORY, checked);
}

void ProjectWidget::slot_buildFinished()
{
    QMessageBox::information(this, windowTitle(), "Сборка завершена");
}

void ProjectWidget::slot_buildCancelled()
{
    QMessageBox::information(this, windowTitle(), "Сборка отменена пользователем");
}

void ProjectWidget::slot_itemsChecked(const QModelIndexList &selected, const Qt::CheckState checkState)
{
    if (selected.empty())
        return;
    undoStack->push(new ItemsCheckedCmd(selected, checkState, project_model));
}

void ProjectWidget::slot_resultHolderChecked(const QModelIndex &index)
{
    undoStack->push(new ResultHolderCheckedCmd(index, project_model));
}
