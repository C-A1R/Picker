#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>

class FileSystemListView;

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

    const FileSystemListView * const view;

public:
    FileSystemModel(const FileSystemListView * const view, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // FILESYSTEMMODEL_H
