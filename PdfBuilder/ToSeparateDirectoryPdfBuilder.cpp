#include "ToSeparateDirectoryPdfBuilder.h"

ToSeparateDirectoryPdfBuilder::ToSeparateDirectoryPdfBuilder(QString &&defenitFolder)
    : separateDirectory{std::move(defenitFolder)}
{
    connect(this, &AbstractPdfBuilder::signal_allFilesProcessed, this, &IPdfBuilder::signal_finished);
}

QString ToSeparateDirectoryPdfBuilder::destinationFilePath(const QString &parentPath)
{
    if (separateDirectory.isEmpty())
    {
        return QString();
    }
    QString titleFileName = AbstractPdfBuilder::findTitleFileName(parentPath);
    QString destination;
    if (!titleFileName.isEmpty())
    {
        destination = separateDirectory + titleFileName.remove("Титул ");
    }
    else
    {
        if (parentPath.endsWith(':'))
        {
            destination = separateDirectory
                          + parentPath.left(parentPath.size() - parentPath.lastIndexOf(':'))
                          + ".pdf";
        }
        else
        {
            destination = separateDirectory
                          + parentPath.right(parentPath.size() - parentPath.lastIndexOf('/') - 1)
                          + ".pdf";
        }
    }
    return destination;
}
