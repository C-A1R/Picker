#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include "ProjectItem.h"

#include <QAbstractItemModel>

enum Columns
{
    col_Name,
    col_ResultHolder,

    MAX
};


class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT

    std::unique_ptr<ProjectItem>        rootItem;

    QFileIconProvider                   iconProvider;
    QHash<quintptr, Qt::CheckState>     checkedItems;

public:
    Q_DISABLE_COPY_MOVE(ProjectModel)

    explicit ProjectModel(QObject *parent = nullptr);
    ~ProjectModel() override = default;

    QVariant data(const QModelIndex &index, const int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(const int row, const int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    bool setProjectPath(const QString &rootPath);

private:
    [[maybe_unused]] bool scanItem(ProjectItem *item);
    void checkItem(const QModelIndex &index);
    void cleanup();

public slots:
    void slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState);
};

#endif // PROJECTMODEL_H
