#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

    QList<QModelIndex> selected;

public:
    FileSystemModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void slot_selectItem(const QModelIndex &index);
};

#endif // FILESYSTEMMODEL_H
