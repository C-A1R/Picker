#ifndef FILESYSTEMWIDGET_H
#define FILESYSTEMWIDGET_H

#include <QWidget>

class QToolBar;
class QLabel;
class FileSystemListView;
class FileSystemModel;

class FileSystemWidget : public QWidget
{
    Q_OBJECT

    QToolBar *drives_toolBar = nullptr;
    QLabel *currentPath_label = nullptr;
    FileSystemListView *fileSystem_listView = nullptr;
    FileSystemModel *fileSystem_model = nullptr;

public:
    FileSystemWidget(QWidget *parent = nullptr);
    ~FileSystemWidget();

private:
    void initUi();
    void initDriveActions();

private slots:
    void slot_goIn();
    void slot_goUp();
    void slot_changeDrive();
};

#endif // FILESYSTEMWIDGET_H
