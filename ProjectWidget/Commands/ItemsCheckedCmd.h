#ifndef ITEMSCHECKEDCMD_H
#define ITEMSCHECKEDCMD_H

#include "ProjectWidget/ProjectModel.h"

#include <QUndoCommand>

class ProjectModel;

class ItemsCheckedCmd : public QUndoCommand
{
    QList<QModelIndex>  selectedItems;
    Qt::CheckState      checkState;
    ProjectModel        *projectModel {nullptr};

public:
    ItemsCheckedCmd(const QModelIndexList &selected, Qt::CheckState checkState, ProjectModel *model = nullptr);

private:
    void undo() override;

    void redo() override;
};

#endif // ITEMSCHECKEDCMD_H
