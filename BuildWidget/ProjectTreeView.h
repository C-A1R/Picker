#ifndef PROJECTTREEVIEW_H
#define PROJECTTREEVIEW_H

#include <QProxyStyle>
#include <QTreeView>

class ProjectTreeView : public QTreeView
{
    Q_OBJECT

    class ProjectTreeViewStyle: public QProxyStyle
    {
    public:
        ProjectTreeViewStyle(QStyle *style = nullptr) : QProxyStyle(style) {}
        void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    };

    QAction *setChecked_action = nullptr;
    QMenu *contextMenu = nullptr;
    QAbstractItemView::DropIndicatorPosition dropIndicatorPosition;

public:
    ProjectTreeView(QWidget *parent = nullptr);

private:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    QAbstractItemView::DropIndicatorPosition getDropIndicatorPosition(const QPoint &position, const QRect &rect);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void signal_dropped(const QModelIndex &, const QModelIndexList &);
    void signal_added(const QModelIndex &, const QString &);
    void signal_setChecked(const QModelIndexList &selected, const bool checked);

private slots:
    void slot_setChecked(const bool checked);

public slots:
    void slot_expand(const QModelIndexList &indices);
};

#endif // PROJECTTREEVIEW_H
