#ifndef PROGECTDELEGATE_H
#define PROGECTDELEGATE_H

#include <QStyledItemDelegate>

class ProgectDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    const int btnWidth = 18;
    const int btnHeight = 18;
    const int padding = 1;

public:
    ProgectDelegate(QObject *parent = nullptr);

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    QRect removeBtnRect(const QStyleOptionViewItem &option) const;
    QRect browseBtnRect(const QStyleOptionViewItem &option) const;

signals:
    void signal_removeBtnClicked(const QModelIndex &index);
    void signal_browseBtnClicked(const QModelIndex &index);
    void signal_doubleClicked(const QModelIndex &index);
};

#endif // PROGECTDELEGATE_H
