#include "ProgectDelegate.h"

#include "Enums.h"

#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

ProgectDelegate::ProgectDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void ProgectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    painter->save();

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    if (index.data(ProjectRoles::STATUS) != ExistingStatus::MISSED)
    {
        painter->restore();
        return;
    }

    QStyleOptionButton buttonOption;
    buttonOption.state = QStyle::State_Enabled;
    buttonOption.rect = removeBtnRect(option);
    buttonOption.text = "X";
    style->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
    buttonOption.rect = browseBtnRect(option);
    buttonOption.text = "...";
    style->drawControl(QStyle::CE_PushButton, &buttonOption, painter);

    painter->restore();
}

bool ProgectDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model)

    if (event->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (removeBtnRect(option).contains(mouseEvent->pos())
            || browseBtnRect(option).contains(mouseEvent->pos()))
        return false;
        emit signal_doubleClicked(index);
        return true;
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (removeBtnRect(option).contains(mouseEvent->pos()))
            emit signal_removeBtnClicked(index);
        else if (browseBtnRect(option).contains(mouseEvent->pos()))
            emit signal_browseBtnClicked(index);
        return true;
    }
    return false;
}

QRect ProgectDelegate::removeBtnRect(const QStyleOptionViewItem &option) const
{
    const int btnTop = option.rect.center().y() + 1 - btnHeight / 2;
    return {option.rect.right() - btnWidth + padding, btnTop, btnWidth, btnHeight};
}

QRect ProgectDelegate::browseBtnRect(const QStyleOptionViewItem &option) const
{
    const int btnTop = option.rect.center().y() + 1 - btnHeight / 2;
    return {option.rect.right() - btnWidth * 2 + padding + 1, btnTop, btnWidth, btnHeight};
}
