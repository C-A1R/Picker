#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include "ProjectModel.h"

#include <QWidget>

class QLabel;
class ProjectTreeView;
class ProjectItem;
class ProjectSortProxyModel;
class IPdfBuilder;
class SqlMgr;
class QUndoStack;

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

    QLabel                  *currentPath_label {nullptr};
    ProjectTreeView         *project_treeView {nullptr};
    ProjectModel            *project_model {nullptr};

    SaveOpt                     saveOptions{SaveOptions::SAVE_TO_PROJECT_DIRECTORIES};
    QScopedPointer<IPdfBuilder> builder;

    QUndoStack *undoStack {nullptr};

public:
    ProjectWidget(QWidget *parent = nullptr);
    ~ProjectWidget();

    static QAction *createOpenAction(const bool isDarkTheme, QObject *parent = nullptr);
    static QAction *createSaveAction(const bool isDarkTheme, QObject *parent = nullptr);
    static QAction *createBuildAction(const bool isDarkTheme, QObject *parent = nullptr);
    static QAction *createUndoAction(const bool isDarkTheme, QObject *parent = nullptr);
    static QAction *createRedoAction(const bool isDarkTheme, QObject *parent = nullptr);

private:
    void initUi();
    void openProject(const QString &path);
    QString getDefenitFolder() const;

    void saveProjectTree(SqlMgr &sqlMgr) const;
    void saveProjectItem(const QModelIndex &itemIndex, SqlMgr &sqlMgr) const;
    void saveItemToDB(const QModelIndex &index, SqlMgr &sqlMgr) const;

signals:
    void signal_canUndoChanged(bool canUndo);
    void signal_canRedoChanged(bool canRedo);

public slots:
    void slot_openProject();
    void slot_saveProject();
    void slot_build();
    void slot_undo();
    void slot_redo();

private slots:
    void slot_saveToFoldersOptionChanged(bool checked);
    void slot_saveToDefenitFolderOptionChanged(bool checked);
    void slot_buildFinished();
    void slot_buildCancelled();

    void slot_itemsChecked(const QModelIndexList &selected, const Qt::CheckState checkState);
    void slot_resultHolderChecked(const QModelIndex &index);
};

#endif // PROJECTWIDGET_H
