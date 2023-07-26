#ifndef PROJECTTREEVIEW_H
#define PROJECTTREEVIEW_H

#include <QTreeView>

class ProjectTreeView : public QTreeView
{
    Q_OBJECT

    QAbstractItemView::DropIndicatorPosition dropIndicatorPosition;

public:
    ProjectTreeView(QWidget *parent = nullptr);

private:
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    QAbstractItemView::DropIndicatorPosition getDropIndicatorPosition(const QPoint &position, const QRect &rect);

signals:
    void signal_dropped(const QModelIndex &, const QModelIndexList &);
};

#endif // PROJECTTREEVIEW_H
