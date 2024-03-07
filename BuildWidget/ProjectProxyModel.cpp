#include "ProjectProxyModel.h"
#include "ProjectFileSystemModel.h"

#include <QFileSystemModel>

ProjectProxyModel::ProjectProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool ProjectProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const ProjectFileSystemModel *source = static_cast<ProjectFileSystemModel *>(sourceModel());
    const QModelIndex &index = source->index(sourceRow, ProjectFileSystemModel::Columns::col_Name, sourceParent);
    return !source->getHiddenIndices().contains(index.internalId());
}

bool ProjectProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const ProjectFileSystemModel *source = static_cast<ProjectFileSystemModel *>(sourceModel());
    const QList<quintptr> &orders = source->getOrders();
    return orders.indexOf(source_left.internalId()) < orders.indexOf(source_right.internalId());
}

void ProjectProxyModel::slot_dropped(const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices)
{
    if (draggedIndices.isEmpty())
    {
        return;
    }
    QList<quintptr> draggedIndicesIds;
    std::transform(draggedIndices.cbegin(), draggedIndices.cend(), std::back_inserter(draggedIndicesIds),
                    [this](const QModelIndex &ind) -> quintptr
                    {
                       return mapToSource(ind).internalId();
                    });
    emit signal_dropped(droppedIndex.isValid() ? mapToSource(droppedIndex).internalId() : 0, draggedIndicesIds);
}

void ProjectProxyModel::slot_added(const QModelIndex &droppedIndex, const QString &fullPaths)
{
    if (fullPaths.isEmpty())
    {
        return;
    }

    // const QStringList paths = fullPaths.split('*');
    // int beginRow = rowCount();
    // qDebug() << "insert:" << insertRows(beginRow, paths.count(), QModelIndex());
    // for (const QString &text : std::as_const(paths))
    // {
    //     QModelIndex idx = index(beginRow, 0, QModelIndex());
    //     setData(idx, text);
    //     beginRow++;
    // }
    emit signal_added(droppedIndex.isValid() ? mapToSource(droppedIndex).internalId() : 0, fullPaths);
}

void ProjectProxyModel::slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState)
{
    if (selected.isEmpty())
    {
        return;
    }
    QModelIndexList sourceIndices;
    std::transform(selected.cbegin(), selected.cend(), std::back_inserter(sourceIndices),
                    [this](const QModelIndex &ind) -> QModelIndex
                    {
                        return mapToSource(ind);
                    });
    emit signal_setChecked(sourceIndices, checkState);
}

void ProjectProxyModel::slot_expand(const QModelIndexList &expanded)
{
    if (expanded.isEmpty())
    {
        return;
    }
    QModelIndexList proxyIndices;
    std::transform(expanded.cbegin(), expanded.cend(), std::back_inserter(proxyIndices),
                   [this](const QModelIndex &ind) -> QModelIndex
                   {
                       return mapFromSource(ind);
                   });
    emit signal_expand(proxyIndices);
}
