#include "ProgectDelegate.h"

#include "ProjectEnums.h"

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

    if (index.data(Project::ItemRole::TYPE).value<Project::Type>() == Project::Type::LINK)
    {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setBrush(Qt::green);
        painter->setPen(Qt::NoPen);
        const QPoint circleCenter = option.rect.bottomLeft() + QPoint(29, -5);
        painter->drawEllipse(circleCenter, 3, 3);
    }

    if (index.data(Project::ItemRole::STATUS) != Project::ExStatus::MISSED)
    {
        painter->restore();
        return;
    }

    {
        QStyleOptionButton removeBtnOpt;
        removeBtnOpt.state = QStyle::State_Enabled;
        removeBtnOpt.rect = removeBtnRect(option);
        removeBtnOpt.icon = QIcon(":/project/ico/crossmark.svg");
        removeBtnOpt.iconSize = {8,8};
        style->drawControl(QStyle::CE_PushButton, &removeBtnOpt, painter);
    }
    {
        QStyleOptionButton browseBtnOpt;
        browseBtnOpt.state = QStyle::State_Enabled;
        browseBtnOpt.rect = browseBtnRect(option);
        browseBtnOpt.text = "...";
        style->drawControl(QStyle::CE_PushButton, &browseBtnOpt, painter);
    }

    painter->restore();
}

bool ProgectDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model)

    if (event->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (ProgectDelegate::removeBtnRect(option).contains(mouseEvent->pos())
            || ProgectDelegate::browseBtnRect(option).contains(mouseEvent->pos()))
        return false;
        emit signal_doubleClicked(index);
        return true;
    }
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (ProgectDelegate::removeBtnRect(option).contains(mouseEvent->pos()))
            emit signal_removeBtnClicked(index);
        else if (ProgectDelegate::browseBtnRect(option).contains(mouseEvent->pos()))
            emit signal_browseBtnClicked(index);
        return true;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect ProgectDelegate::removeBtnRect(const QStyleOptionViewItem &option)
{
    const int btnTop = option.rect.center().y() + 1 - btnHeight / 2;
    return {option.rect.right() - btnWidth + padding, btnTop, btnWidth, btnHeight};
}

QRect ProgectDelegate::browseBtnRect(const QStyleOptionViewItem &option)
{
    const int btnTop = option.rect.center().y() + 1 - btnHeight / 2;
    return {option.rect.right() - btnWidth * 2 + padding + 1, btnTop, btnWidth, btnHeight};
}
