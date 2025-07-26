#include "FileSystemView.h"

#include "FileSystemModel.h"

#include <QApplication>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QShortcut>

FileSystemView::FileSystemView(QWidget *parent)
    : QTableView(parent)
{
    new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(slot_selectItem()));
}

const QSet<QModelIndex> &FileSystemView::getSelected() const
{
    return selected;
}

void FileSystemView::selectItem(const QModelIndex &index)
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

void FileSystemView::selectItemRow(const QModelIndex &index)
{
    for(int i = 0; i < model()->columnCount(); ++i)
        selectItem(model()->index(index.row(), i, index.parent()));
}

void FileSystemView::mousePressEvent(QMouseEvent *event)
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
        selectItemRow(curr);
    }
    QTableView::mousePressEvent(event);
}

void FileSystemView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!event)
    {
        return;
    }
    if (event->buttons() & Qt::RightButton)
    {
        selectInstruction = SelectInstructions::do_nothing;
    }
    QTableView::mouseReleaseEvent(event);
}

void FileSystemView::mouseMoveEvent(QMouseEvent *event)
{
    if (!event)
    {
        return;
    }
    if (event->buttons() & Qt::RightButton)
    {
        selectItemRow(indexAt(event->pos()));
        event->ignore();
        return;
    }
    if (event->buttons() & Qt::LeftButton)
    {
        selectInstruction = SelectInstructions::do_select;
        selectItemRow(currentIndex());

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

void FileSystemView::mouseDoubleClickEvent(QMouseEvent *event)
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

void FileSystemView::slot_selectItem()
{
    const QModelIndex &currIndex = currentIndex();
    selectInstruction = selected.contains(currIndex) ? SelectInstructions::do_unselect
                                                     : SelectInstructions::do_select;
    selectItemRow(currIndex);
    setCurrentIndex(currIndex.sibling(currIndex.row() + 1, currIndex.column()));
}
