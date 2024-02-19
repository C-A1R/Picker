#ifndef BUILDWIDGET_H
#define BUILDWIDGET_H

#include <QWidget>
#include <QBitArray>

#include "ProjectModel.h"

class QToolBar;
class QLabel;
class QProgressDialog;
class ProjectTreeView;
class ProjectProxyModel;
class IPdfBuilder;
class SqlMgr;

using p_model_type = ProjectModel;

class BuildWidget : public QWidget
{
    Q_OBJECT

    enum SaveOptions
    {
        SAVE_NONE = 0x0,
        SAVE_TO_PROJECT_DIRECTORIES = 0x1,
        SAVE_TO_SEPARATE_DIRECTORY = 0x2
    };
    Q_DECLARE_FLAGS(SaveOpt, SaveOptions);

    QToolBar            *actions_toolBar {nullptr};
    QToolBar            *saveOptions_toolBar {nullptr};
    QLabel              *currentPath_label {nullptr};
    ProjectTreeView     *project_treeView {nullptr};
    p_model_type        *project_model {nullptr};
    ProjectProxyModel   *proxy_model {nullptr};

    SaveOpt                     saveOptions{SaveOptions::SAVE_TO_PROJECT_DIRECTORIES};
    QScopedPointer<IPdfBuilder> builder;

public:
    BuildWidget(QWidget *parent = nullptr);
    ~BuildWidget();

private:
    void initUi();
    void changeProject(const QString &path);
    QString getDefenitFolder() const;
    void saveTree(const QModelIndex &rootIndex, SqlMgr &sqlMgr) const;

private slots:
    void slot_changeProject();
    void slot_saveList();
    void slot_build();
    void slot_saveToFoldersOptionChanged(bool checked);
    void slot_saveToDefenitFolderOptionChanged(bool checked);
    void slot_buildFinished();
    void slot_buildCancelled();
};

#endif // BUILDWIDGET_H
