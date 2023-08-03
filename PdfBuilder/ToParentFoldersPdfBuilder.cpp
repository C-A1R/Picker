#include "ToParentFoldersPdfBuilder.h"

ToParentFoldersPdfBuilder::ToParentFoldersPdfBuilder(const QString &rootPath)
    : AbstractPdfBuilder{rootPath}
{
    connect(this, &AbstractPdfBuilder::signal_allFilesProcessed, this, &IPdfBuilder::signal_finished);
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
