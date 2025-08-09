#ifndef FILEEXPLORERENUMS_H
#define FILEEXPLORERENUMS_H

#include <QFileSystemModel>

namespace FileExplorer
{

enum Column
{
    col_Name,
    col_Size,
    col_Type,
    col_LastModified,

    col_Max
};

enum ItemRole
{
    ABS_PATH = QFileSystemModel::Roles::FilePathRole
};

} // namespace FileExplorer

#endif // FILEEXPLORERENUMS_H
