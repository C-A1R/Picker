#include "FileSystemListView.h"

#include "FileSystemModel.h"

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

const QSet<QModelIndex> &FileSystemListView::getSelected() const
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
        selected.insert(index);
    }
    else
    {
        selected.remove(index);
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
        selectInstruction = selected.contains(curr) ? SelectInstructions::do_unselect
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
        selectInstruction = SelectInstructions::do_select;
        selectItem(currentIndex());

        const FileSystemModel *fsModel = static_cast<fs_model_type *>(model());
        QStringList paths;
        for (const QModelIndex &ind : std::as_const(selected))
        {
            paths.emplace_back(fsModel->filePath(ind));
        }
        if (paths.empty())
        {
            event->ignore();
            return;
        }
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(paths.join('*'));
        drag->setMimeData(mimeData);
        drag->exec();
    }
    selected.clear();
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
    selectInstruction = selected.contains(currIndex) ? SelectInstructions::do_unselect
                                                     : SelectInstructions::do_select;
    selectItem(currIndex);
    setCurrentIndex(currIndex.sibling(currIndex.row() + 1, currIndex.column()));
}
