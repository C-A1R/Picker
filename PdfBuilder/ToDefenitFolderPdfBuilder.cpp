#include "ToDefenitFolderPdfBuilder.h"

ToDefenitFolderPdfBuilder::ToDefenitFolderPdfBuilder(const QString &rootPath, QString &&defenitFolder)
    : AbstractPdfBuilder(rootPath), defenitFolder{std::move(defenitFolder)}
{
}

QString ToDefenitFolderPdfBuilder::destinationFilePath(const QString &parentPath)
{
    if (defenitFolder.isEmpty())
    {
        return QString();
    }
    QString titleFileName = AbstractPdfBuilder::findTitleFileName(parentPath);
    QString destination;
    if (!titleFileName.isEmpty())
    {
        destination = defenitFolder + titleFileName.remove("Титул ");
    }
    else
    {
        destination = defenitFolder
                      + parentPath.right(parentPath.size() - parentPath.lastIndexOf('/') - 1)
                      + ".pdf";
    }
    return destination;
}