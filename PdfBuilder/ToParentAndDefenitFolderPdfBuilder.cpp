#include "ToParentAndDefenitFolderPdfBuilder.h"

#include <QDir>

ToParentAndDefenitFolderPdfBuilder::ToParentAndDefenitFolderPdfBuilder(const QString &rootPath, QString &&defenitFolder)
    : ToParentFoldersPdfBuilder(rootPath), defenitFolder{std::move(defenitFolder)}
{
    disconnect(this, &AbstractPdfBuilder::signal_allFilesProcessed, this, &IPdfBuilder::signal_finished);
    connect(this, &AbstractPdfBuilder::signal_allFilesProcessed, this, &ToParentAndDefenitFolderPdfBuilder::slot_allFilesProcessed);
}

void ToParentAndDefenitFolderPdfBuilder::exec(const QStringList &paths)
{
    destinations.clear();
    ToParentFoldersPdfBuilder::exec(paths);
}

QString ToParentAndDefenitFolderPdfBuilder::destinationFilePath(const QString &parentPath)
{
    const QString dest = ToParentFoldersPdfBuilder::destinationFilePath(parentPath);
    destinations << dest;
    return dest;
}

void ToParentAndDefenitFolderPdfBuilder::slot_allFilesProcessed()
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
