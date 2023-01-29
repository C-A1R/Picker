#ifndef FILESYSTEMWIDGET_H
#define FILESYSTEMWIDGET_H

#include <QWidget>

class QToolBar;
class QLabel;
class QListView;
class FileSystemModel;

class FileSystemWidget : public QWidget
{
    Q_OBJECT

    QToolBar *drivePanel = nullptr;
    QLabel *currentPath_label = nullptr;
    QListView *fileSystem_listView = nullptr;
    FileSystemModel *model = nullptr;
public:
    FileSystemWidget(QWidget *parent = nullptr);

private:
    void initUi();
    void initDriveActions();

private slots:
    void slot_goIn();
    void slot_goUp();
    void slot_changeDrive();
};

#endif // FILESYSTEMWIDGET_H
