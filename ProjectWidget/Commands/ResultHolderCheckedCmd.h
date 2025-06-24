#ifndef RESULTHOLDERCHECKEDCMD_H
#define RESULTHOLDERCHECKEDCMD_H

#include "ProjectWidget/ProjectModel.h"

#include <QUndoCommand>

class ResultHolderCheckedCmd : public QUndoCommand
{
    QModelIndex         index;
    ProjectModel        *projectModel {nullptr};
    Qt::CheckState      checkState;
    QModelIndexList     resultHolders;

public:
    ResultHolderCheckedCmd(const QModelIndex &index, ProjectModel *model = nullptr);

private:
    void undo() override;
    void redo() override;
};

#endif // RESULTHOLDERCHECKEDCMD_H
