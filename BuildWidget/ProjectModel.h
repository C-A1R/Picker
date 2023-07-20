#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QFileSystemModel>

class ProjectModel : public QFileSystemModel
{
public:
    ProjectModel(QObject *parent = nullptr);

//    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
//    QModelIndex parent(const QModelIndex &child) const override;
//    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
//    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // PROJECTMODEL_H
