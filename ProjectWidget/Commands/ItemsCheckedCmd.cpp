#include "ItemsCheckedCmd.h"


ItemsCheckedCmd::ItemsCheckedCmd(const QModelIndexList &selected, Qt::CheckState checkState, ProjectModel *model)
    : selectedItems(selected)
    , checkState(checkState)
    , projectModel(model)
{
}

void ItemsCheckedCmd::undo()
{
    if (!projectModel)
        return;
    projectModel->setItemsChecked(selectedItems, checkState == Qt::Checked ? Qt::Unchecked : Qt::Checked);
}

void ItemsCheckedCmd::redo()
{
    if (!projectModel)
        return;
    projectModel->setItemsChecked(selectedItems, checkState);
}
