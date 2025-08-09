#include "ProjectTreeView.h"
#include "ProjectEnums.h"
#include "ProjectItem.h"
#include "ProgectDelegate.h"

#include <QDropEvent>
#include <QMenu>
#include <QPainter>
#include <QMimeData>
#include <QToolTip>

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent)
{
    setStyle(new ProjectTreeViewStyle(style()));

    auto projectDelegate = new ProgectDelegate(this);
    setItemDelegateForColumn(Project::Column::col_Name, projectDelegate);
    connect(projectDelegate, &ProgectDelegate::signal_removeBtnClicked, this, &ProjectTreeView::signal_itemRemoveBtnClicked);
    connect(projectDelegate, &ProgectDelegate::signal_browseBtnClicked, this, &ProjectTreeView::signal_itemBrowseBtnClicked);
    connect(projectDelegate, &ProgectDelegate::signal_doubleClicked,    this, &ProjectTreeView::signal_itemDoubleClicked);
}

void ProjectTreeView::mousePressEvent(QMouseEvent *event)
{
    const QModelIndex index = indexAt(event->pos());
    if (index.isValid() && event->button() == Qt::LeftButton)
    {
        if (!checkBoxClicked(index, event))
        {
            QTreeView::mousePressEvent(event);
            return;
        }
        if (index.column() == Project::Column::col_ResultHolder)
        {
            emit signal_resultHolderChecked(index);
            return;
        }

        QModelIndexList selected = selectedIndexes();
        selected.removeIf([&index](const QModelIndex &ind)
                          {
                              return ind.column() != index.column();
                          });
        if (selected.isEmpty())
        {
            selected << index;
            selectRow(index);
        }
        else if (!selected.contains(index))
        {
            selected.clear();
            selected << index;
            selectionModel()->clearSelection();
            selectRow(index);
        }

        const auto currCheckState = index.data(Qt::CheckStateRole).value<Qt::CheckState>();
        const auto newCheckState = (currCheckState == Qt::Checked || currCheckState == Qt::PartiallyChecked) ? Qt::Unchecked : Qt::Checked;
        selected.removeIf([&newCheckState](const QModelIndex &ind)
                          {
                              return ind.data(Qt::CheckStateRole) == newCheckState;
                          });
        emit signal_itemsChecked(selected, newCheckState);
        return;
    }
    QTreeView::mousePressEvent(event);
}

void ProjectTreeView::mouseReleaseEvent(QMouseEvent *event)
{
    const QModelIndex index = indexAt(event->pos());
    if (index.isValid() && event->button() == Qt::LeftButton)
    {
        if (checkBoxClicked(index, event))
            return;
    }
    QTreeView::mouseReleaseEvent(event);
}

void ProjectTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event)
    {
        return;
    }
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
    QModelIndex dropRootIndex = droppedIndex.parent();
    if (dropIndicatorPosition == QAbstractItemView::DropIndicatorPosition::BelowItem)
    {
        droppedIndex = droppedIndex.siblingAtRow(droppedIndex.row() + 1);
    }

    QSet<qulonglong> expandedIds;
    getExpandedItemIds(rootIndex(), expandedIds);
    if (event->source() == this && event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) // from this
    {
        QModelIndexList draggedIndices = this->selectedIndexes();
        if (draggedIndices.isEmpty())
        {
            return;
        }
        if (droppedIndex.isValid() && droppedIndex == draggedIndices.first())
        {
            return;
        }
        draggedIndices.removeIf([](const QModelIndex &index) { return index.column() != Project::Column::col_Name; });
        emit signal_dropped(dropRootIndex, droppedIndex, draggedIndices);
    }
    else if (event->mimeData()->hasFormat("text/plain")) // from left panel
    {
        emit signal_added(dropRootIndex, droppedIndex, event->mimeData()->text());
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
            expandedIds.insert(static_cast<const ProjectItem*>(index.internalPointer())->id());
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
        if (expandedIds.contains(item->id()))
            expand(index);
        else
            return;
    }
    for (int i = 0; i < model()->rowCount(index); ++i)
    {
        expandItems(model()->index(i, 0, index), expandedIds);
    }
}

bool ProjectTreeView::checkBoxClicked(const QModelIndex &index, QMouseEvent *event) const
{
    if (!index.isValid())
        return false;
    QStyleOptionButton opt;
    opt.rect = visualRect(index);
    QRect checkBoxRect = style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt);
    {//костыль:
        checkBoxRect.setLeft(checkBoxRect.left() + 4);
        checkBoxRect.setRight(checkBoxRect.right() + 4);
    }
    return checkBoxRect.contains(event->pos());
}

void ProjectTreeView::selectRow(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    for (int col = 0; col < Project::Column::MAX; ++col)
    {
        selectionModel()->select(model()->index(index.row(), col, index.parent()), QItemSelectionModel::Select);
    }
}

bool ProjectTreeView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        QModelIndex index = indexAt(helpEvent->pos());
        if (!index.isValid())
            return QTreeView::viewportEvent(event);
        if (index.data(Project::ItemRole::STATUS) != Project::ExStatus::MISSED)
            return QTreeView::viewportEvent(event);

        QStyleOptionViewItem option;
        option.initFrom(this);
        option.rect = visualRect(index);
        if (ProgectDelegate::removeBtnRect(option).contains(helpEvent->pos()))
        {
            QToolTip::showText(helpEvent->globalPos(), "Удалить элемент");
            return true;
        }
        else if (ProgectDelegate::browseBtnRect(option).contains(helpEvent->pos()))
        {
            QToolTip::showText(helpEvent->globalPos(), "Найти элемент");
            return true;
        }
    }

    return QTreeView::viewportEvent(event);
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
