#ifndef PROJECTTREEVIEW_H
#define PROJECTTREEVIEW_H

#include <QProxyStyle>
#include <QTreeView>

/**
 * @brief The ProjectTreeView class
 * Дерево проекта
 */
class ProjectTreeView : public QTreeView
{
    Q_OBJECT

    class ProjectTreeViewStyle: public QProxyStyle
    {
    public:
        ProjectTreeViewStyle(QStyle *style = nullptr) : QProxyStyle(style) {}
        void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    };

    QAbstractItemView::DropIndicatorPosition dropIndicatorPosition;

public:
    ProjectTreeView(QWidget *parent = nullptr);

private:
    void mouseReleaseEvent (QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    QAbstractItemView::DropIndicatorPosition getDropIndicatorPosition(const QPoint &position, const QRect &rect);

signals:
    void signal_dropped(const QModelIndex &, const QModelIndexList &);
    void signal_added(const QModelIndex &, const QString &);
    void signal_setChecked(const QModelIndexList &, const Qt::CheckState);

public slots:
    void slot_expand(const QModelIndexList &indices);
};

#endif // PROJECTTREEVIEW_H
