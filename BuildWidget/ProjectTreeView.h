#ifndef PROJECTTREEVIEW_H
#define PROJECTTREEVIEW_H

#include <QTreeView>

class ProjectTreeView : public QTreeView
{
    Q_OBJECT

    QAction *setChecked_action = nullptr;
    QMenu *contextMenu = nullptr;
    QAbstractItemView::DropIndicatorPosition dropIndicatorPosition;

public:
    ProjectTreeView(QWidget *parent = nullptr);

private:
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    QAbstractItemView::DropIndicatorPosition getDropIndicatorPosition(const QPoint &position, const QRect &rect);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void signal_dropped(const QModelIndex &, const QModelIndexList &);
    void signal_setChecked(const QModelIndexList &selected, const bool checked);

private slots:
    void slot_setChecked(const bool checked);
};

#endif // PROJECTTREEVIEW_H
