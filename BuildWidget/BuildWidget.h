#ifndef BUILDWIDGET_H
#define BUILDWIDGET_H

#include <QWidget>
#include <QBitArray>

class QToolBar;
class QLabel;
class ProjectTreeView;
class ProjectModel;
class ProjectProxyModel;


class BuildWidget : public QWidget
{
    enum SaveOptions
    {
        SAVE_NONE = 0x0,
        SAVE_TO_FOLDERS = 0x1,
        SAVE_TO_DEFENIT_FOLDER = 0x2
    };
    Q_DECLARE_FLAGS(SaveOpt, SaveOptions);

    Q_OBJECT

    QToolBar *actions_toolBar = nullptr;
    QToolBar *saveOptions_toolBar = nullptr;
    QLabel *currentPath_label = nullptr;
    ProjectTreeView *project_treeView = nullptr;
    ProjectModel *project_model = nullptr;
    ProjectProxyModel *proxy_model = nullptr;

    SaveOpt saveOptions = SaveOptions::SAVE_TO_FOLDERS;

public:
    BuildWidget(QWidget *parent = nullptr);
    ~BuildWidget();

private:
    void initUi();
    void changeProject(const QString &path);

private slots:
    void slot_changeProject();
    void slot_saveList();
    void slot_build();
    void slot_saveToFoldersOptionChanged(bool checked);
    void slot_saveToDefenitFolderOptionChanged(bool checked);
};

#endif // BUILDWIDGET_H
