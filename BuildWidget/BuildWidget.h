#ifndef BUILDWIDGET_H
#define BUILDWIDGET_H

#include <QWidget>

class QToolBar;
class QLabel;
class QTreeView;
class ProjectModel;

class BuildWidget : public QWidget
{
    Q_OBJECT

    QToolBar *actions_toolBar = nullptr;
    QLabel *currentPath_label = nullptr;
    QTreeView *project_treeView = nullptr;
    ProjectModel *project_model = nullptr;

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
};

#endif // BUILDWIDGET_H
