#include "FileSystemListView.h"

#include <QApplication>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QShortcut>

FileSystemListView::FileSystemListView(QWidget *parent)
    : QListView(parent)
{
    new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(slot_selectItem()));
}

const QSet<qintptr> &FileSystemListView::getSelected() const
{
    return selected;
}

void FileSystemListView::selectItem(const QModelIndex &index)
{
    if (selectInstruction == SelectInstructions::do_nothing)
    {
        return;
    }

    if (selectInstruction == SelectInstructions::do_select)
    {
        selected.insert(index.internalId());
    }
    else
    {
        selected.remove(index.internalId());
    }
    update(index);
}

void FileSystemListView::mousePressEvent(QMouseEvent *event)
{
    if (!event)
    {
        return;
    }
    if (event->buttons() & Qt::RightButton)
    {
        const QModelIndex &curr = indexAt(event->pos());
        selectInstruction = selected.contains(curr.internalId()) ? SelectInstructions::do_unselect
                                                                 : SelectInstructions::do_select;
        selectItem(curr);
    }
    QListView::mousePressEvent(event);
}

void FileSystemListView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!event)
    {
        return;
    }
    if (event->buttons() & Qt::RightButton)
    {
        selectInstruction = SelectInstructions::do_nothing;
    }
    QListView::mouseReleaseEvent(event);
}

void FileSystemListView::mouseMoveEvent(QMouseEvent *event)
{
    if (!event)
    {
        return;
    }
    if (event->buttons() & Qt::RightButton)
    {
        selectItem(indexAt(event->pos()));
        event->ignore();
        return;
    }
    if (event->buttons() & Qt::LeftButton)
    {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText("test");
        drag->setMimeData(mimeData);
        drag->exec();
    }
}

void FileSystemListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!event)
    {
        return;
    }
    if (event->buttons() &~ Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    emit doubleClicked(currentIndex());
}

void FileSystemListView::slot_selectItem()
{
    const QModelIndex &currIndex = currentIndex();
    selectInstruction = selected.contains(currIndex.internalId()) ? SelectInstructions::do_unselect
                                                                  : SelectInstructions::do_select;
    selectItem(currIndex);
    setCurrentIndex(currIndex.sibling(currIndex.row() + 1, currIndex.column()));
}
