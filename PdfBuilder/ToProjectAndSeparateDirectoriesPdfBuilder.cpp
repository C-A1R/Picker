#include "ToProjectAndSeparateDirectoriesPdfBuilder.h"

#include <QDir>
#include <QProgressDialog>

ToProjectAndSeparateDirectoryPdfBuilder::ToProjectAndSeparateDirectoryPdfBuilder(QString &&defenitFolder)
    : defenitFolder{std::move(defenitFolder)}
{
    disconnect(this, &AbstractPdfBuilder::signal_allFilesProcessed, this, &IPdfBuilder::signal_finished);
    connect(this, &AbstractPdfBuilder::signal_allFilesProcessed, this, &ToProjectAndSeparateDirectoryPdfBuilder::slot_allFilesProcessed);
}

void ToProjectAndSeparateDirectoryPdfBuilder::exec(const QHash<QString, QStringList> &fileStructure)
{
    destinations.clear();
    ToProjectDirectoriesPdfBuilder::exec(fileStructure);
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
    QScopedPointer<QProgressDialog> progress(new QProgressDialog("Копирование...", "Отмена", currentProgress, 0));
    progress->setWindowModality(Qt::ApplicationModal);
    progress->setMaximum(destinations.count());
    progress->show();
    for (const auto &dest : std::as_const(destinations))
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
