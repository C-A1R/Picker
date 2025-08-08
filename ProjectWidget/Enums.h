#ifndef ENUMS_H
#define ENUMS_H

#include <qnamespace.h>

enum ProlectColumns
{
    col_Name,
    col_LastModified,
    col_ResultHolder,

    MAX
};

enum ProjectRoles
{
    ID = Qt::UserRole,
    STATUS,
    ABS_PATH
};

enum ExistingStatus
{
    DEFAULT = 0,
    LISTED,
    NOT_LISTED,
    MISSED
};

#endif // ENUMS_H
