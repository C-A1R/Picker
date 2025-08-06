#ifndef FILEEXPLORERWIDGET_H
#define FILEEXPLORERWIDGET_H

#include <QWidget>

class QToolBar;
class QLabel;
class FileSystemView;
class FileSystemModel;

class FileExplorerWidget : public QWidget
{
    Q_OBJECT

    QToolBar            *drives_toolBar = nullptr;
    QLabel              *currentPath_label = nullptr;
    FileSystemView      *view = nullptr;
    FileSystemModel     *model = nullptr;

public:
    FileExplorerWidget(QWidget *parent = nullptr);
    ~FileExplorerWidget();

private:
    void initUi();
    void initDriveActions();

private slots:
    void slot_doubleClicked();
    void slot_goIn();
    void slot_goUp();
    void slot_changeDrive();
};

#endif // FILEEXPLORERWIDGET_H
