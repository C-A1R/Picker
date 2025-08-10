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
    STATUS,     ///<ExStatus
    ABS_PATH,
    TYPE        ///<Type
};

enum ExStatus //status of existing
{
    DEFAULT = 0,
    LISTED,
    NOT_LISTED,
    MISSED
};

enum Type
{
    LOCAL_ELEMENT,
    LINK
};

} // namespace Project

#endif // PROJECTENUMS_H
