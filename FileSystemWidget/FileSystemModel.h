#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>

class FileSystemView;

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

    const FileSystemView * const view;

public:
    enum Columns
    {
        col_Name,
        col_Size,
        col_Type,
        col_Date,

        col_Max
    };

    FileSystemModel(const FileSystemView * const view, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
};

#endif // FILESYSTEMMODEL_H
