#ifndef PROJECTPROXYMODEL_H
#define PROJECTPROXYMODEL_H

#include <QSortFilterProxyModel>

class QDir;
class ProjectModel;

class ProjectProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ProjectProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

signals:
    void signal_dropped(const quintptr, const QList<quintptr>);

public slots:
    void slot_dropped(const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices);
};

#endif // PROJECTPROXYMODEL_H
