#include "ToFoldersPdfBuilder.h"

#include <QDebug>

void ToFoldersPdfBuilder::exec(const QStringList &paths) const
{
    for (const QString &path : paths)
    {
        qDebug() << "#" << path;
    }
}
