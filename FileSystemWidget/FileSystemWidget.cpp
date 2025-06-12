#include "FileSystemWidget.h"
#include "FileSystemListView.h"
#include "FileSystemModel.h"
#include "Settings.h"

#include <QToolBar>
#include <QLabel>
#include <QShortcut>
#include <QVBoxLayout>
#include <QFileSystemModel>

FileSystemWidget::FileSystemWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();
    initDriveActions();
    connect(fileSystem_listView, &FileSystemListView::doubleClicked, this, &FileSystemWidget::slot_goIn);
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

    fileSystem_listView = new FileSystemListView(this);
    fileSystem_listView->setAlternatingRowColors(true);
    fileSystem_listView->setDragEnabled(true);
    fileSystem_listView->setDefaultDropAction(Qt::IgnoreAction);
    fileSystem_listView->setDragDropMode(QAbstractItemView::DragOnly);
    fileSystem_model = new FileSystemModel(fileSystem_listView, this);
    fileSystem_model->setFilter(QDir::AllEntries | QDir::AllDirs | QDir::NoDot);
    fileSystem_model->setNameFilters(QStringList() << "*.pdf");
    fileSystem_model->setNameFilterDisables(false);
    fileSystem_listView->setModel(fileSystem_model);

    auto main_vLay = new QVBoxLayout();
    main_vLay->setContentsMargins(0, 0, 0, 0);
    main_vLay->setSpacing(0);
    main_vLay->addWidget(drives_toolBar);
    main_vLay->addWidget(currentPath_label);
    main_vLay->addSpacing(3);
    main_vLay->addWidget(fileSystem_listView);
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
        act->setIcon(fileSystem_model->fileIcon(fileSystem_model->index(drive.path())));
        act->setIconText(drive.path());
        act->setCheckable(true);
        connect(act, &QAction::triggered, this, &FileSystemWidget::slot_changeDrive);
        drives_toolBar->addAction(act);
    }
    auto setDefaultFileSystem = [this, &drives]()
    {
        const QString &drivePath = drives.first().path();
        fileSystem_model->setRootPath(drivePath);
        fileSystem_listView->setRootIndex(fileSystem_model->index(drivePath));
        drives_toolBar->actions().at(0)->setChecked(true);
        currentPath_label->setText(drivePath);
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
    fileSystem_model->setRootPath(lastPath);
    fileSystem_listView->setRootIndex(fileSystem_model->index(lastPath));
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
}

void FileSystemWidget::slot_goIn()
{
    const QModelIndex &index = fileSystem_listView->currentIndex();
    if (!fileSystem_model->isDir(index))
    {
        return;
    }
    const QString &newRootPath = fileSystem_model->fileInfo(index).filePath();
    if (newRootPath.endsWith(".."))
    {
        slot_goUp();
        return;
    }
    fileSystem_listView->setRootIndex(fileSystem_model->index(newRootPath));
    fileSystem_model->setRootPath(newRootPath);
    currentPath_label->setText(index.data(QFileSystemModel::FilePathRole).toString());
    fileSystem_listView->setCurrentIndex(QModelIndex());
}

void FileSystemWidget::slot_goUp()
{
    const QModelIndex &parentIndex = fileSystem_listView->rootIndex().parent();
    fileSystem_listView->setRootIndex(parentIndex);
    const QString prevRootPath = fileSystem_model->rootPath();
    fileSystem_model->setRootPath(fileSystem_model->fileInfo(parentIndex).filePath());
    currentPath_label->setText(parentIndex.data(QFileSystemModel::FilePathRole).toString());
    fileSystem_listView->setCurrentIndex(fileSystem_model->index(prevRootPath));
}

void FileSystemWidget::slot_changeDrive()
{
    for (QAction *act : drives_toolBar->actions())
    {
        act->setChecked(false);
    }
    auto act = static_cast<QAction *>(sender());
    act->setChecked(true);
    fileSystem_model->setRootPath(act->text());
    fileSystem_listView->setRootIndex(fileSystem_model->index(act->text()));
    currentPath_label->setText(fileSystem_model->rootPath());
}
