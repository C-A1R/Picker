#ifndef PROGECTDELEGATE_H
#define PROGECTDELEGATE_H

#include <QStyledItemDelegate>

class ProgectDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    static const int btnWidth   = 18;
    static const int btnHeight  = 18;
    static const int padding    = 1;

public:
    ProgectDelegate(QObject *parent = nullptr);
    static QRect removeBtnRect(const QStyleOptionViewItem &option);
    static QRect browseBtnRect(const QStyleOptionViewItem &option);

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void signal_removeBtnClicked(const QModelIndex &index);
    void signal_browseBtnClicked(const QModelIndex &index);
    void signal_doubleClicked(const QModelIndex &index);
};

#endif // PROGECTDELEGATE_H
