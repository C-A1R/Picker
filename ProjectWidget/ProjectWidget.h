#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QWidget>

class QToolBar;
class QLabel;
class ProjectTreeView;
class ProjectModel;
class ProjectItem;
class ProjectSortProxyModel;
class IPdfBuilder;
class SqlMgr;

/**
 * @brief The ProjectWidget class
 * Виджет для сборки проекта
 */
class ProjectWidget : public QWidget
{
    Q_OBJECT

    enum SaveOptions
    {
        SAVE_NONE = 0x0,
        SAVE_TO_PROJECT_DIRECTORIES = 0x1,
        SAVE_TO_SEPARATE_DIRECTORY = 0x2
    };
    Q_DECLARE_FLAGS(SaveOpt, SaveOptions);

    QToolBar                *actions_toolBar {nullptr};
    QToolBar                *saveOptions_toolBar {nullptr};
    QLabel                  *currentPath_label {nullptr};
    ProjectTreeView         *project_treeView {nullptr};
    ProjectModel            *project_model {nullptr};

    SaveOpt                     saveOptions{SaveOptions::SAVE_TO_PROJECT_DIRECTORIES};
    QScopedPointer<IPdfBuilder> builder;

public:
    ProjectWidget(QWidget *parent = nullptr);
    ~ProjectWidget();

private:
    void initUi();
    void changeProject(const QString &path);
    QString getDefenitFolder() const;

    void saveProjectTree(const std::shared_ptr<const ProjectItem> &rootItem, SqlMgr &sqlMgr) const;
    void saveProjectItem(const QModelIndex &itemIndex, SqlMgr &sqlMgr) const;
    void saveItem(const QModelIndex &index, SqlMgr &sqlMgr) const;

private slots:
    void slot_changeProject();
    void slot_saveProject();
    void slot_build();
    void slot_saveToFoldersOptionChanged(bool checked);
    void slot_saveToDefenitFolderOptionChanged(bool checked);
    void slot_buildFinished();
    void slot_buildCancelled();
};

#endif // PROJECTWIDGET_H
