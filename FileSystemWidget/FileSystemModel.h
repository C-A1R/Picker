#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>

class FileSystemModel : public QFileSystemModel
{
public:
    FileSystemModel(QObject *parent = nullptr);
};

#endif // FILESYSTEMMODEL_H
