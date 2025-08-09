#ifndef PROJECTENUMS_H
#define PROJECTENUMS_H

#include <qnamespace.h>

namespace Project
{

enum Column
{
    col_Name,
    col_LastModified,
    col_ResultHolder,

    MAX
};

enum ItemRole
{
    ID = Qt::UserRole,
    STATUS,
    ABS_PATH
};

enum ExStatus //status of existing
{
    DEFAULT = 0,
    LISTED,
    NOT_LISTED,
    MISSED
};

} // namespace Project

#endif // PROJECTENUMS_H
