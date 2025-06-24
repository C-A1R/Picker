#include "ResultHolderCheckedCmd.h"

ResultHolderCheckedCmd::ResultHolderCheckedCmd(const QModelIndex &index, ProjectModel *model)
    : index(index)
    , projectModel(model)
{
    if (!projectModel)
        return;

    const auto currCheckState = index.data(Qt::CheckStateRole).value<Qt::CheckState>();
    checkState = (currCheckState == Qt::Checked || currCheckState == Qt::PartiallyChecked) ? Qt::Unchecked : Qt::Checked;
    resultHolders = projectModel->getResultHolderIndices(index);
}

void ResultHolderCheckedCmd::undo()
{
    if (!projectModel)
        return;

    const Qt::CheckState oldCheckState = checkState == Qt::Checked ? Qt::Unchecked : Qt::Checked;
    projectModel->setData(index, oldCheckState, Qt::CheckStateRole);
    projectModel->setResultHolders(resultHolders);
}

void ResultHolderCheckedCmd::redo()
{
    if (!projectModel)
        return;
    projectModel->setData(index, checkState, Qt::CheckStateRole);
}
