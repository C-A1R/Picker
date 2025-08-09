#include "FileExplorerWidget.h"
#include "FileSystemView.h"
#include "FileSystemModel.h"
#include "Settings.h"
#include "FileExplorerEnums.h"

#include <QToolBar>
#include <QLabel>
#include <QShortcut>
#include <QVBoxLayout>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QDesktopServices>


FileExplorerWidget::FileExplorerWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();
    initDriveActions();
    connect(view, &FileSystemView::doubleClicked, this, &FileExplorerWidget::slot_doubleClicked);
    new QShortcut(QKeySequence(Qt::Key_Return), this, SLOT(slot_goIn()));
    new QShortcut(QKeySequence(Qt::Key_Enter), this, SLOT(slot_goIn()));
    new QShortcut(QKeySequence(Qt::Key_Backspace), this, SLOT(slot_goUp()));
}

FileExplorerWidget::~FileExplorerWidget()
{
    Settings::instance()->setValue(SETTINGS_FILE_EXP_PATH, currentPath_label->text());
}

void FileExplorerWidget::initUi()
{
    drives_toolBar = new QToolBar(this);
    drives_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    currentPath_label = new QLabel(this);
    QFont boldFont;
    boldFont.setBold(true);
    currentPath_label->setFont(boldFont);
    currentPath_label->setIndent(5);
    currentPath_label->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);

    view = new FileSystemView(this);
    model = new FileSystemModel(view, this);
    model->setFilter(QDir::AllEntries | QDir::AllDirs | QDir::NoDot);
    model->setNameFilters(QStringList() << "*.pdf");
    model->setNameFilterDisables(false);
    view->setModel(model);
    view->setAlternatingRowColors(true);
    view->setDragEnabled(true);
    view->setDefaultDropAction(Qt::IgnoreAction);
    view->setDragDropMode(QAbstractItemView::DragOnly);
    view->setSelectionBehavior(FileSystemView::SelectRows);
    view->horizontalHeader()->hide();
    view->horizontalHeader()->setStretchLastSection(false);
    view->horizontalHeader()->setSectionResizeMode(FileExplorer::Column::col_Name, QHeaderView::Stretch);
    view->horizontalHeader()->setSectionResizeMode(FileExplorer::Column::col_LastModified, QHeaderView::ResizeToContents);
    const int rowHeight = 20;
    view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    view->verticalHeader()->setDefaultSectionSize(rowHeight);
    view->verticalHeader()->setMaximumSectionSize(rowHeight);
    view->verticalHeader()->setMinimumSectionSize(rowHeight);
    view->verticalHeader()->hide();
    view->hideColumn(FileExplorer::Column::col_Size);
    view->hideColumn(FileExplorer::Column::col_Type);
    view->setShowGrid(false);
    view->resizeRowsToContents();

    auto main_vLay = new QVBoxLayout();
    main_vLay->setContentsMargins(0, 0, 0, 0);
    main_vLay->setSpacing(0);
    main_vLay->addWidget(drives_toolBar);
    main_vLay->addWidget(currentPath_label);
    main_vLay->addSpacing(3);
    main_vLay->addWidget(view);
    setLayout(main_vLay);
}

void FileExplorerWidget::initDriveActions()
{
    const QFileInfoList &drives = QDir::drives();
    if (drives.empty())
    {
        return;
    }
    for (const QFileInfo &drive : drives)
    {
        auto act = new QAction(drive.path(), drives_toolBar);
        act->setIcon(model->fileIcon(model->index(drive.path())));
        act->setIconText(drive.path());
        act->setCheckable(true);
        connect(act, &QAction::triggered, this, &FileExplorerWidget::slot_changeDrive);
        drives_toolBar->addAction(act);
    }
    auto setDefaultFileSystem = [this, &drives]()
    {
        const QString &drivePath = drives.first().path();
        model->setRootPath(drivePath);
        view->setRootIndex(model->index(drivePath));
        drives_toolBar->actions().at(0)->setChecked(true);
        currentPath_label->setText(drivePath);
        currentPath_label->setToolTip(drivePath);
    };

    const auto lastPath{Settings::instance()->value(SETTINGS_FILE_EXP_PATH).toString()};
    if (lastPath.isEmpty())
    {
        setDefaultFileSystem();
        return;
    }
    QDir lastDir{lastPath};
    if (!lastDir.exists())
    {
        setDefaultFileSystem();
        return;
    }

    auto driveIter = std::find_if(drives.cbegin(), drives.cend(), [&lastPath](const QFileInfo &d) -> bool
    {
        return lastPath.startsWith(d.filePath());
    });
    if (driveIter == drives.cend())
    {
        setDefaultFileSystem();
        return;
    }

    const QString &drivePath = (*driveIter).path();
    model->setRootPath(lastPath);
    view->setRootIndex(model->index(lastPath));
    const auto &actions = drives_toolBar->actions();
    auto actIter = std::find_if(actions.cbegin(), actions.cend(), [&drivePath](const QAction *act) -> bool
    {
        return act->text() == drivePath;
    });
    if (actIter != actions.cend())
    {
        (*actIter)->setChecked(true);
    }
    currentPath_label->setText(lastPath);
    currentPath_label->setToolTip(lastPath);
}

void FileExplorerWidget::slot_doubleClicked()
{
    QModelIndex index = view->currentIndex();
    if (model->isDir(index))
    {
        slot_goIn();
        return;
    }

    if (index.column() != FileExplorer::Column::col_Name)
        index = index.siblingAtColumn(FileExplorer::Column::col_Name);

    QDesktopServices::openUrl(QUrl::fromLocalFile(model->filePath(index)));
}

void FileExplorerWidget::slot_goIn()
{
    const QModelIndex &index = view->currentIndex();
    if (!model->isDir(index))
    {
        return;
    }
    const QString &newRootPath = model->fileInfo(index).filePath();
    if (newRootPath.endsWith(".."))
    {
        slot_goUp();
        return;
    }
    view->clearSelected();
    view->setRootIndex(model->index(newRootPath));
    model->setRootPath(newRootPath);
    const QString &drivePath = index.data(QFileSystemModel::FilePathRole).toString();
    currentPath_label->setText(drivePath);
    currentPath_label->setToolTip(drivePath);
    view->setCurrentIndex(QModelIndex());
}

void FileExplorerWidget::slot_goUp()
{
    view->clearSelected();
    const QModelIndex &parentIndex = view->rootIndex().parent();
    view->setRootIndex(parentIndex);
    const QString prevRootPath = model->rootPath();
    model->setRootPath(model->fileInfo(parentIndex).filePath());
    const QString &drivePath = parentIndex.data(QFileSystemModel::FilePathRole).toString();
    currentPath_label->setText(drivePath);
    currentPath_label->setToolTip(drivePath);
    view->setCurrentIndex(model->index(prevRootPath));
}

void FileExplorerWidget::slot_changeDrive()
{
    for (QAction *act : drives_toolBar->actions())
    {
        act->setChecked(false);
    }
    auto act = static_cast<QAction *>(sender());
    act->setChecked(true);
    model->setRootPath(act->text());
    view->setRootIndex(model->index(act->text()));
    currentPath_label->setText(model->rootPath());
    currentPath_label->setToolTip(model->rootPath());
}
