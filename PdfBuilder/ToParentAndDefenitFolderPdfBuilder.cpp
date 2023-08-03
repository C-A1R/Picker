#include "ToParentAndDefenitFolderPdfBuilder.h"

#include <QDir>

ToParentAndDefenitFolderPdfBuilder::ToParentAndDefenitFolderPdfBuilder(const QString &rootPath, QString &&defenitFolder)
    : ToParentFoldersPdfBuilder(rootPath), defenitFolder{std::move(defenitFolder)}
{
}

void ToParentAndDefenitFolderPdfBuilder::exec(const QStringList &paths)
{
    destinations.clear();
    ToParentFoldersPdfBuilder::exec(paths);
    for (const auto &dest : destinations)
    {
        QFile::copy(dest, defenitFolder + dest.right(dest.size() - dest.lastIndexOf('/')));
    }
}

QString ToParentAndDefenitFolderPdfBuilder::destinationFilePath(const QString &parentPath)
{
    const QString dest = ToParentFoldersPdfBuilder::destinationFilePath(parentPath);
    destinations << dest;
    return dest;
}
