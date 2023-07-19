#include "FileSystemWidget.h"

#include <QToolBar>
#include <QLabel>
#include <QListView>
#include <QShortcut>
#include <QVBoxLayout>
#include <QFileSystemModel>

FileSystemWidget::FileSystemWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();
    initDriveActions();
    connect(fileSystem_listView, &QListView::doubleClicked, this, &FileSystemWidget::slot_goIn);
    new QShortcut(QKeySequence(Qt::Key_Return), this, SLOT(slot_goIn()));
    new QShortcut(QKeySequence(Qt::Key_Enter), this, SLOT(slot_goIn()));
    new QShortcut(QKeySequence(Qt::Key_Backspace), this, SLOT(slot_goUp()));
}

void FileSystemWidget::initUi()
{
    drivePanel = new QToolBar(this);
    drivePanel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    currentPath_label = new QLabel(this);
    QFont boldFont;
    boldFont.setBold(true);
    currentPath_label->setFont(boldFont);
    currentPath_label->setIndent(5);
    currentPath_label->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);

    fileSystem_listView = new QListView(this);
    fileSystem_listView->setAlternatingRowColors(true);
    model = new QFileSystemModel(this);
    model->setFilter(QDir::AllEntries | QDir::NoDot);
    fileSystem_listView->setModel(model);

    auto main_vLay = new QVBoxLayout();
    main_vLay->setContentsMargins(0, 0, 0, 0);
    main_vLay->setSpacing(0);
    main_vLay->addWidget(drivePanel);
    main_vLay->addWidget(currentPath_label);
    main_vLay->addSpacing(3);
    main_vLay->addWidget(fileSystem_listView);
    setLayout(main_vLay);
}

void FileSystemWidget::initDriveActions()
{
    const QFileInfoList drives = QDir::drives();
    if (drives.empty())
    {
        return;
    }
    model->setRootPath(drives.first().path());
    fileSystem_listView->setRootIndex(model->index(drives.first().path()));
    for (const QFileInfo &drive : drives)
    {
        auto act = new QAction(drive.path(), drivePanel);
        act->setIcon(model->fileIcon(model->index(drive.path())));
        act->setIconText(drive.path());
        act->setCheckable(true);
        connect(act, &QAction::triggered, this, &FileSystemWidget::slot_changeDrive);
        drivePanel->addAction(act);
    }
    drivePanel->actions().at(0)->setChecked(true);
    currentPath_label->setText(model->rootPath());
}

void FileSystemWidget::slot_goIn()
{
    const auto index = fileSystem_listView->currentIndex();
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
    fileSystem_listView->setRootIndex(model->index(newRootPath));
    currentPath_label->setText(index.data(QFileSystemModel::FilePathRole).toString());
}

void FileSystemWidget::slot_goUp()
{
    if (fileSystem_listView->rootIndex().data(QFileSystemModel::FilePathRole) == model->rootPath())
    {
        return;
    }
    const auto parentIndex = fileSystem_listView->rootIndex().parent();
    fileSystem_listView->setRootIndex(parentIndex);
    currentPath_label->setText(parentIndex.data(QFileSystemModel::FilePathRole).toString());
}

void FileSystemWidget::slot_changeDrive()
{
    for (QAction *act : drivePanel->actions())
    {
        act->setChecked(false);
    }
    auto act = static_cast<QAction *>(sender());
    act->setChecked(true);
    model->setRootPath(act->text());
    fileSystem_listView->setRootIndex(model->index(act->text()));
    currentPath_label->setText(model->rootPath());
}
