#include "ProjectTreeView.h"

#include <QDropEvent>
#include <QMenu>
#include <QPainter>

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent)
{
    setStyle(new ProjectTreeViewStyle(style()));
    {
        setChecked_action = new QAction("Отметить", this);
        setChecked_action->setCheckable(true);
        connect(setChecked_action, &QAction::triggered, this, &ProjectTreeView::slot_setChecked);
    }
    contextMenu = new QMenu(this);
    contextMenu->addAction(setChecked_action);
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
    this->sortByColumn(0, Qt::AscendingOrder);
    event->accept();
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

void ProjectTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    if (!event)
    {
        return;
    }
    bool checked = true;
    const QModelIndexList &selected = selectedIndexes();
    for (const auto &index : selectedIndexes())
    {
        if (!index.data(Qt::CheckStateRole).toBool())
        {
            checked = false;
            break;
        }
    }
    setChecked_action->setChecked(checked);
    contextMenu->exec(event->globalPos());
}

void ProjectTreeView::slot_setChecked(const bool checked)
{
    emit signal_setChecked(selectedIndexes(), checked);
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
