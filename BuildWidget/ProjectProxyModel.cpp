#include "ProjectProxyModel.h"
#include "ProjectModel.h"
#include "ProjectItem.h"


ProjectProxyModel::ProjectProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    qDebug() << "ProjectProxyModel constructed";

    // Применим сортировку при любых изменениях
    setDynamicSortFilter(true);
    setFilterKeyColumn(-1);  // Проверять все колонки
}

bool ProjectProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // Разрешаем все строки (можно заменить на логику фильтрации по имени, типу и т.п.)
    return true;
}

bool ProjectProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return sourceModel()->data(left).toString() < sourceModel()->data(right).toString();

    // const QString leftText = sourceModel()->data(left, Qt::DisplayRole).toString();
    // const QString rightText = sourceModel()->data(right, Qt::DisplayRole).toString();
    // return QString::localeAwareCompare(leftText, rightText) < 0;
}

QModelIndex ProjectProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return QSortFilterProxyModel::mapToSource(proxyIndex);
}

QModelIndex ProjectProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

bool ProjectProxyModel::hasChildren(const QModelIndex &parent) const
{
    const QModelIndex sourceParent = mapToSource(parent);
    return sourceModel()->hasChildren(sourceParent);
}

int ProjectProxyModel::rowCount(const QModelIndex &parent) const
{
    int count = QSortFilterProxyModel::rowCount(parent);
    qDebug() << "proxy rowCount at" << parent << " = " << count;
    return count;
}















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
