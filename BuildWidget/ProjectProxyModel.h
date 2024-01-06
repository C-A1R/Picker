#ifndef PROJECTPROXYMODEL_H
#define PROJECTPROXYMODEL_H

#include <QSortFilterProxyModel>

class ProjectProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ProjectProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

signals:
    void signal_dropped(const quintptr, const QList<quintptr> &);
    void signal_added(const quintptr, const QString &);
    void signal_setChecked(const QModelIndexList &, const bool);
    void signal_expand(const QModelIndexList &) const;

public slots:
    void slot_dropped(const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices);
    void slot_added(const QModelIndex &droppedIndex, const QString &fullPaths);
    void slot_setChecked(const QModelIndexList &selected, const bool checked);
    void slot_expand(const QModelIndexList &expanded) const;
};

#endif // PROJECTPROXYMODEL_H
