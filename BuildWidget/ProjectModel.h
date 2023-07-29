#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QFileSystemModel>

class TreeNode;

class ProjectModel : public QFileSystemModel
{
    Q_OBJECT

    QHash<quintptr, Qt::CheckState> checkedItems;
    QSet<quintptr> hiddenIndexes;
    QList<quintptr> orders;
    QHash<quintptr, QString> pdfPaths;

public:
    ProjectModel(QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::DropActions supportedDropActions() const override;

    const QSet<quintptr> &getHiddenIndexes() const;
    const QList<quintptr> &getOrders() const;
    const QStringList getCheckedPdfPaths() const;

private:
    [[maybe_unused]] bool scanForHiddenItems(const QDir &dir);
    void scanOrder(const QDir &dir);
    void cleanup();

signals:
    void signal_itemChecked(const QModelIndex&);

public slots:
    void slot_dropped(const quintptr droppedIndexId, const QList<quintptr> draggeddIndicesIds);
    void slot_setChecked(const QModelIndexList &selected, const bool checked);

protected slots:
    void slot_onItemChecked(const QModelIndex& index);
    void slot_onRootPathChanged();
};

#endif // PROJECTMODEL_H
