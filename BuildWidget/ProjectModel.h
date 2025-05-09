#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include "ProjectItem.h"

#include <QAbstractItemModel>

enum Columns
{
    filename,

    MAX
};


class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT

    std::unique_ptr<ProjectItem> rootItem;

public:
    Q_DISABLE_COPY_MOVE(ProjectModel)

    explicit ProjectModel(QObject *parent = nullptr);
    ~ProjectModel() override = default;

    QVariant data(const QModelIndex &index, const int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(const int section, Qt::Orientation orientation, const int role = Qt::DisplayRole) const override;
    QModelIndex index(const int row, const int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    void setProjectPath(const QString &rootPath);

private:
    void scanItem(ProjectItem *item);
};

#endif // PROJECTMODEL_H
