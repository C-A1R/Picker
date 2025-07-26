#include "FileSystemWidget.h"
#include "FileSystemView.h"
#include "FileSystemModel.h"
#include "Settings.h"

#include <QToolBar>
#include <QLabel>
#include <QShortcut>
#include <QVBoxLayout>
#include <QFileSystemModel>
#include <QHeaderView>

FileSystemWidget::FileSystemWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();

    initDriveActions();
    connect(view, &FileSystemView::doubleClicked, this, &FileSystemWidget::slot_goIn);
    new QShortcut(QKeySequence(Qt::Key_Return), this, SLOT(slot_goIn()));
    new QShortcut(QKeySequence(Qt::Key_Enter), this, SLOT(slot_goIn()));
    new QShortcut(QKeySequence(Qt::Key_Backspace), this, SLOT(slot_goUp()));
}

FileSystemWidget::~FileSystemWidget()
{
    Settings::instance()->setValue(SETTINGS_FILESYSTEM_PATH, currentPath_label->text());
}

void FileSystemWidget::initUi()
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
    view->horizontalHeader()->setSectionResizeMode(FileSystemModel::Columns::col_Name, QHeaderView::Stretch);
    view->horizontalHeader()->setSectionResizeMode(FileSystemModel::Columns::col_LastModified, QHeaderView::ResizeToContents);
    view->verticalHeader()->hide();
    view->verticalHeader()->setDefaultSectionSize(5);
    view->hideColumn(FileSystemModel::Columns::col_Size);
    view->hideColumn(FileSystemModel::Columns::col_Type);

    auto main_vLay = new QVBoxLayout();
    main_vLay->setContentsMargins(0, 0, 0, 0);
    main_vLay->setSpacing(0);
    main_vLay->addWidget(drives_toolBar);
    main_vLay->addWidget(currentPath_label);
    main_vLay->addSpacing(3);
    main_vLay->addWidget(view);
    setLayout(main_vLay);
}

void FileSystemWidget::initDriveActions()
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
        connect(act, &QAction::triggered, this, &FileSystemWidget::slot_changeDrive);
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

    const auto lastPath{Settings::instance()->value(SETTINGS_FILESYSTEM_PATH).toString()};
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

void FileSystemWidget::slot_goIn()
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
    view->setRootIndex(model->index(newRootPath));
    model->setRootPath(newRootPath);
    const QString &drivePath = index.data(QFileSystemModel::FilePathRole).toString();
    currentPath_label->setText(drivePath);
    currentPath_label->setToolTip(drivePath);
    view->setCurrentIndex(QModelIndex());
}

void FileSystemWidget::slot_goUp()
{
    const QModelIndex &parentIndex = view->rootIndex().parent();
    view->setRootIndex(parentIndex);
    const QString prevRootPath = model->rootPath();
    model->setRootPath(model->fileInfo(parentIndex).filePath());
    const QString &drivePath = parentIndex.data(QFileSystemModel::FilePathRole).toString();
    currentPath_label->setText(drivePath);
    currentPath_label->setToolTip(drivePath);
    view->setCurrentIndex(model->index(prevRootPath));
}

void FileSystemWidget::slot_changeDrive()
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
