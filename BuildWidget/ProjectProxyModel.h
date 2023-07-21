#ifndef PROJECTPROXYMODEL_H
#define PROJECTPROXYMODEL_H

#include <QSortFilterProxyModel>

class QDir;
class ProjectModel;

class ProjectProxyModel : public QSortFilterProxyModel
{
public:
    ProjectProxyModel(QObject *parent = nullptr);
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // PROJECTPROXYMODEL_H
