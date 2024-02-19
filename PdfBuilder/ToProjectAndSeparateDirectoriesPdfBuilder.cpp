#include "ToProjectAndSeparateDirectoriesPdfBuilder.h"

#include <QDir>

ToProjectAndSeparateDirectoryPdfBuilder::ToProjectAndSeparateDirectoryPdfBuilder(QStringList &&resultHolderPaths, QString &&defenitFolder)
    : ToProjectDirectoriesPdfBuilder{std::move(resultHolderPaths)}
    , defenitFolder{std::move(defenitFolder)}
{
    disconnect(this, &AbstractPdfBuilder::signal_allFilesProcessed, this, &IPdfBuilder::signal_finished);
    connect(this, &AbstractPdfBuilder::signal_allFilesProcessed, this, &ToProjectAndSeparateDirectoryPdfBuilder::slot_allFilesProcessed);
}

void ToProjectAndSeparateDirectoryPdfBuilder::exec(const QStringList &paths)
{
    destinations.clear();
    ToProjectDirectoriesPdfBuilder::exec(paths);
}

QString ToProjectAndSeparateDirectoryPdfBuilder::destinationFilePath(const QString &parentPath)
{
    const QString dest = ToProjectDirectoriesPdfBuilder::destinationFilePath(parentPath);
    destinations << dest;
    return dest;
}

void ToProjectAndSeparateDirectoryPdfBuilder::slot_allFilesProcessed()
{
    int currentProgress = 0;
    progress->setValue(currentProgress);
    progress->setMaximum(destinations.count());
    for (const auto &dest : destinations)
    {
        if (progress->wasCanceled())
        {
            emit signal_cancelled();
            return;
        }
        QFile::copy(dest, defenitFolder + dest.right(dest.size() - dest.lastIndexOf('/')));
        progress->setValue(++currentProgress);
    }
    emit signal_finished();
}
