#include "ToParentFoldersPdfBuilder.h"

ToParentFoldersPdfBuilder::ToParentFoldersPdfBuilder(const QString &rootPath)
    : AbstractPdfBuilder{rootPath}
{
}

QString ToParentFoldersPdfBuilder::destinationFilePath(const QString &parentPath)
{
    QString titleFileName = AbstractPdfBuilder::findTitleFileName(parentPath);
    QString destination;
    if (!titleFileName.isEmpty())
    {
        destination = parentPath + '/' + titleFileName.remove("Титул ");
    }
    else
    {
        destination = parentPath
                 + parentPath.right(parentPath.size() - parentPath.lastIndexOf('/'))
                 + ".pdf";
    }
    return destination;
}
