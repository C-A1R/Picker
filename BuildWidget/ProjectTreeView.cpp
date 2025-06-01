#include "ProjectTreeView.h"
#include "Enums.h"
#include "ProjectItem.h"

#include <QDropEvent>
#include <QMenu>
#include <QPainter>
#include <QMimeData>

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent)
{
    setStyle(new ProjectTreeViewStyle(style()));
}

void ProjectTreeView::mouseReleaseEvent(QMouseEvent *event)
{
    const QModelIndex index = indexAt(event->pos());
    if (index.isValid()
        && index.column() == Columns::col_Name
        && event->button() == Qt::LeftButton)
    {
         QModelIndexList selected = selectedIndexes();
        if (!selected.contains(index))
        {
            QTreeView::mouseReleaseEvent(event);
            return;
        }
        QStyleOptionButton opt;
        opt.rect = visualRect(index);
        QRect checkBoxRect = style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt);
        {//костыль:
            checkBoxRect.setLeft(checkBoxRect.left() + 4);
            checkBoxRect.setRight(checkBoxRect.right() + 4);
        }
        if (checkBoxRect.contains(event->pos()))
        {
            const auto currCheckState = index.data(Qt::CheckStateRole).value<Qt::CheckState>();
            emit signal_setChecked(selected, (currCheckState == Qt::Checked || currCheckState == Qt::PartiallyChecked) ? Qt::Unchecked : Qt::Checked);
            return;
        }
    }
    QTreeView::mouseReleaseEvent(event);
}

void ProjectTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event)
    {
        return;
    }
    auto f = event->mimeData()->formats();
    // if (event->mimeData()->hasFormat("text/uri-list")) // from this
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) // from this
    {
        if (event->source() != this)
        {
            event->ignore();
            return;
        }
        event->accept();
    }
    else if (event->mimeData()->hasFormat("text/plain")) // from left panel
    {
        if (event->source() == this)
        {
            event->ignore();
            return;
        }
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
        return;
    }
}

void ProjectTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (!event)
    {
        return;
    }
    const QModelIndex &droppedIndex = indexAt(event->position().toPoint());
    dropIndicatorPosition = getDropIndicatorPosition(event->position().toPoint(), visualRect(droppedIndex));

    if (dropIndicatorPosition == QAbstractItemView::DropIndicatorPosition::AboveItem
        || dropIndicatorPosition == QAbstractItemView::DropIndicatorPosition::BelowItem)
    {
        if (const QModelIndexList &draggedIndices = this->selectedIndexes();
            !draggedIndices.isEmpty())
        {
            if (droppedIndex.isValid() && draggedIndices.first().parent().internalId() != droppedIndex.parent().internalId())
            {
                event->ignore();
                return;
            }
        }
        QTreeView::dragMoveEvent(event);
        event->accept();
        return;
    }
    event->ignore();
}

void ProjectTreeView::dropEvent(QDropEvent *event)
{
    if (!event)
    {
        return;
    }
    if (dropIndicatorPosition != QAbstractItemView::DropIndicatorPosition::AboveItem
        && dropIndicatorPosition != QAbstractItemView::DropIndicatorPosition::BelowItem)
    {
        event->ignore();
        return;
    }
    QModelIndex droppedIndex = indexAt(event->position().toPoint());
    if (dropIndicatorPosition == QAbstractItemView::DropIndicatorPosition::BelowItem)
    {
        droppedIndex = model()->index(droppedIndex.row() + 1, droppedIndex.column(), droppedIndex.parent());
    }

    QSet<qulonglong> expandedIds;
    getExpandedItemIds(rootIndex(), expandedIds);
    // if (event->source() == this && event->mimeData()->hasFormat("text/uri-list")) // from this
    if (event->source() == this && event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) // from this
    {
        const QModelIndexList &draggedIndices = this->selectedIndexes();
        if (draggedIndices.isEmpty())
        {
            return;
        }
        if (droppedIndex.isValid() && droppedIndex == draggedIndices.first())
        {
            return;
        }
        emit signal_dropped(droppedIndex, draggedIndices);
    }
    else if (event->mimeData()->hasFormat("text/plain")) // from left panel
    {
        emit signal_added(droppedIndex, event->mimeData()->text());
    }
    event->accept();
    this->selectionModel()->clearSelection();
    expandItems(rootIndex(), expandedIds);
}

QAbstractItemView::DropIndicatorPosition ProjectTreeView::getDropIndicatorPosition(const QPoint &position, const QRect &rect)
{
    const int margin = 2;
    if (position.y() - rect.top() < margin)
    {
        return DropIndicatorPosition::AboveItem;
    }
    if (rect.bottom() - position.y() < margin)
    {
        return DropIndicatorPosition::BelowItem;
    }
    if (rect.contains(position, true))
    {
        return DropIndicatorPosition::OnItem;
    }
    return DropIndicatorPosition::OnViewport;
}

void ProjectTreeView::getExpandedItemIds(const QModelIndex &index, QSet<qulonglong> &expandedIds) const
{
    const ProjectItem *item = static_cast<const ProjectItem*>(index.internalPointer());
    if (item)
    {
        if (isExpanded(index))
            expandedIds.insert(static_cast<const ProjectItem*>(index.internalPointer())->getId());
        else
            return;
    }
    for (int i = 0; i < model()->rowCount(index); ++i)
    {
        getExpandedItemIds(model()->index(i, 0, index), expandedIds);
    }
}

void ProjectTreeView::expandItems(const QModelIndex &index, const QSet<qulonglong> &expandedIds)
{
    const ProjectItem *item = static_cast<const ProjectItem*>(index.internalPointer());
    if (item)
    {
        if (expandedIds.contains(item->getId()))
            expand(index);
        else
            return;
    }
    for (int i = 0; i < model()->rowCount(index); ++i)
    {
        expandItems(model()->index(i, 0, index), expandedIds);
    }
}

void ProjectTreeView::slot_expand(const QModelIndexList &indices)
{
    for (const QModelIndex &ind : indices)
    {
        expand(ind);
    }
}

void ProjectTreeView::ProjectTreeViewStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::PE_IndicatorItemViewItemDrop && !option->rect.isNull())
    {
        QRect rect(option->rect);
        rect.setLeft(0);
        rect.setHeight(3);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(QBrush(QColor(44, 62, 80)));
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(rect, 1.5, 1.5);
        painter->restore();
        return;
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}
