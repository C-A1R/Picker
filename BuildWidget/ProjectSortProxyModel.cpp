#include "ProjectSortProxyModel.h"
#include "ProjectItem.h"

#include <algorithm>

ProjectSortProxyModel::ProjectSortProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

int ProjectSortProxyModel::rowCount(const QModelIndex &parent) const
{
    QModelIndex srcParent = mapToSource(parent);
    int count = sourceModel()->rowCount(srcParent);
    resort(parent); // гарантируем, что список отсортирован
    return count;
}

QModelIndex ProjectSortProxyModel::index(const int row, const int column, const QModelIndex &parent) const
{
    resort(parent);
    QModelIndex srcParent = mapToSource(parent);
    int sourceRow = row;
    auto it = m_sortedRows.constFind(srcParent);
    if (it != m_sortedRows.constEnd() && row < it->size())
        sourceRow = it->at(row);
    QModelIndex srcIdx = sourceModel()->index(sourceRow, column, srcParent);
    return createIndex(row, column, srcIdx.internalPointer());
}

QVariant ProjectSortProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    QModelIndex srcIdx = mapToSource(proxyIndex);
    return sourceModel()->data(srcIdx, role);
}

void ProjectSortProxyModel::sort(const int column, const Qt::SortOrder order)
{
    m_sortColumn = column;
    m_sortOrder = order;
    m_sortedRows.clear();
    // layoutChanged, чтобы виджеты обновились
    emit layoutChanged();
}

/**
 * @brief ProjectSortProxyModel::compareRows
 * @return
 * -1 если a < b
 * 1 если a > b
 * 0 если a == b
 */
int ProjectSortProxyModel::compareRows(const QModelIndex &a, const QModelIndex &b) const
{
    const ProjectItem *itemA = static_cast<ProjectItem*>(a.internalPointer());
    const ProjectItem *itemB = static_cast<ProjectItem*>(b.internalPointer());

    if (!itemA->isDir() && itemB->isDir())
        return 1;
    else if (itemA->isDir() && !itemB->isDir())
        return -1;

    QVariant va = sourceModel()->data(a.siblingAtColumn(m_sortColumn));
    QVariant vb = sourceModel()->data(b.siblingAtColumn(m_sortColumn));
    int cmp = QString::localeAwareCompare(va.toString(), vb.toString());
    return (m_sortOrder == Qt::AscendingOrder) ? cmp : -cmp;
}

void ProjectSortProxyModel::resort(const QModelIndex &parent) const
{
    QModelIndex srcParent = mapToSource(parent);
    if (m_sortedRows.contains(srcParent))
        return; // уже отсортировано

    int count = sourceModel()->rowCount(srcParent);
    QVector<int> rows(count);
    for (int i = 0; i < count; ++i)
    {
        rows[i] = i;
    }

    // Сортируем индексы согласно compareRows
    std::sort(rows.begin(), rows.end(), [&](int a, int b)
    {
        QModelIndex ia = sourceModel()->index(a, m_sortColumn, srcParent);
        QModelIndex ib = sourceModel()->index(b, m_sortColumn, srcParent);
        return compareRows(ia, ib) < 0;
    });

    m_sortedRows[srcParent] = rows;
}



/*********************************************************************************************************************************************/

// QVariant ProjectProxyModel::data(const QModelIndex &index, const int role) const
// {
//     return sourceModel()->data(mapToSource(index), role);
// }

// bool ProjectProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
// {
//     qDebug() << "[filterAcceptsRow] row =" << sourceRow << ", parent =" << sourceParent;

//     Q_UNUSED(sourceRow)
//     Q_UNUSED(sourceParent)
//     return true;

//     // return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
//     // const ProjectFileSystemModel *source = static_cast<ProjectFileSystemModel *>(sourceModel());
//     // const QModelIndex &index = source->index(sourceRow, ProjectFileSystemModel::Columns::col_Name, sourceParent);
//     // return !source->getHiddenIndices().contains(index.internalId());
// }

// bool ProjectProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
// {
//     return static_cast<ProjectItem*>(mapFromSource(source_left).internalPointer())->getPath().dirName()
//            < static_cast<ProjectItem*>(mapFromSource(source_right).internalPointer())->getPath().dirName();
//     // const ProjectModel *source = static_cast<ProjectModel *>(sourceModel());
//     // const QList<quintptr> &orders = source->getOrders();
//     // return orders.indexOf(source_left.internalId()) < orders.indexOf(source_right.internalId());
// }

// bool ProjectProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
// {
//     auto *leftItem = static_cast<ProjectItem*>(source_left.internalPointer());
//     auto *rightItem = static_cast<ProjectItem*>(source_right.internalPointer());

//     if (!leftItem || !rightItem)
//         return false;

//     return leftItem->getPath().dirName() < rightItem->getPath().dirName();
// }

// QModelIndex ProjectProxyModel::mapToSource(const QModelIndex &proxyIndex) const
// {
//     if (proxyIndex.row() >= sourceModel()->rowCount())
//     {
//         return createIndex(proxyIndex.row(), proxyIndex.column());
//     }
//     return QSortFilterProxyModel::mapToSource(proxyIndex);
// }

// void ProjectProxyModel::slot_dropped(const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices)
// {
//     if (draggedIndices.isEmpty())
//     {
//         return;
//     }
//     QList<quintptr> draggedIndicesIds;
//     std::transform(draggedIndices.cbegin(), draggedIndices.cend(), std::back_inserter(draggedIndicesIds),
//                     [this](const QModelIndex &ind) -> quintptr
//                     {
//                        return mapToSource(ind).internalId();
//                     });
//     emit signal_dropped(droppedIndex.isValid() ? mapToSource(droppedIndex).internalId() : 0, draggedIndicesIds);
// }

// void ProjectProxyModel::slot_added(const QModelIndex &droppedIndex, const QString &fullPaths)
// {
//     if (fullPaths.isEmpty())
//     {
//         return;
//     }

//     // const QStringList paths = fullPaths.split('*');
//     // int beginRow = rowCount();
//     // qDebug() << "insert:" << insertRows(beginRow, paths.count(), QModelIndex());
//     // for (const QString &text : std::as_const(paths))
//     // {
//     //     QModelIndex idx = index(beginRow, 0, QModelIndex());
//     //     setData(idx, text);
//     //     beginRow++;
//     // }
//     emit signal_added(droppedIndex.isValid() ? mapToSource(droppedIndex).internalId() : 0, fullPaths);
// }

// void ProjectProxyModel::slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState)
// {
//     if (selected.isEmpty())
//     {
//         return;
//     }
//     QModelIndexList sourceIndices;
//     std::transform(selected.cbegin(), selected.cend(), std::back_inserter(sourceIndices),
//                     [this](const QModelIndex &ind) -> QModelIndex
//                     {
//                         return mapToSource(ind);
//                     });
//     emit signal_setChecked(sourceIndices, checkState);
// }

// void ProjectProxyModel::slot_expand(const QModelIndexList &expanded)
// {
//     if (expanded.isEmpty())
//     {
//         return;
//     }
//     QModelIndexList proxyIndices;
//     std::transform(expanded.cbegin(), expanded.cend(), std::back_inserter(proxyIndices),
//                    [this](const QModelIndex &ind) -> QModelIndex
//                    {
//                        return mapFromSource(ind);
//                    });
//     emit signal_expand(proxyIndices);
// }
