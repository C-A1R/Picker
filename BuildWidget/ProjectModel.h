#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QFileSystemModel>

class ProjectModel : public QFileSystemModel
{
    Q_OBJECT

    QHash<qint64, Qt::CheckState> checkedItems;
public:
    ProjectModel(QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
    bool hasChildren(const QModelIndex &parent) const override;

signals:
    void signal_itemChecked(const QModelIndex&);

protected slots:
    void slot_onItemChecked(const QModelIndex& index);
};

#endif // PROJECTMODEL_H
