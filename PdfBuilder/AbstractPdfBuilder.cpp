#include "AbstractPdfBuilder.h"
#include "PDFWriter/PDFWriter.h"

#include <QDir>
#include <QProgressDialog>
#include <QThread>
#include <QFileDialog>
#include <QProgressDialog>

#include <thread>
#include <iostream>

AbstractPdfBuilder::AbstractPdfBuilder()
{
    const unsigned int threads_count = 4/*std::thread::hardware_concurrency() - 1*/;
    threads.reserve(threads_count);
    for (unsigned int i = 0 ; i < threads_count; ++i)
    {
        threads.emplace_back(&AbstractPdfBuilder::loop, this);
    }
}

AbstractPdfBuilder::~AbstractPdfBuilder()
{
    stop();
    for (auto &t : threads)
    {
        t.join();
    }
}

void AbstractPdfBuilder::exec(const QHash<QString, QStringList> &fileStructure)
{
    if (fileStructure.isEmpty())
        return;

    {
        QHashIterator<QString, QStringList> iter(fileStructure);
        while(iter.hasNext())
        {
            iter.next();
            expectedProgress += iter.value().count();
        }
    }

    QSharedPointer<QProgressDialog> progress(new QProgressDialog("Сборка...", "Отмена", currentProgress, 0));
    progress->setWindowModality(Qt::ApplicationModal);
    disconnect(progress.get(), &QProgressDialog::canceled, progress.get(), &QProgressDialog::cancel);
    connect(progress.get(), &QProgressDialog::canceled, this, &AbstractPdfBuilder::slot_cancelled);
    connect(this, &AbstractPdfBuilder::signal_fileProcessed, this,
            [progress, this]()
            {
                progress->setValue(++currentProgress);
                if (currentProgress == expectedProgress)
                {
                    emit signal_allFilesProcessed();
                    progress->accept();
                }
            }, Qt::QueuedConnection);

    progress->setMaximum(expectedProgress);
    progress->show();
    QHashIterator<QString, QStringList> iter(fileStructure);
    while(iter.hasNext())
    {
        iter.next();
        const QStringList pdfFilePaths = iter.value();
        if (pdfFilePaths.isEmpty())
            continue;

        const QString destinationPdfPath = destinationFilePath(iter.key());
        if (destinationPdfPath.isEmpty())
            continue;

        {
            std::unique_lock lock(taskMutex);
            tasks.emplace([pdfFilePaths, destinationPdfPath, this]() -> void
                          {
                              QDir dir{destinationPdfPath};
                              if (dir.exists())
                                  dir.remove(destinationPdfPath);

                              PDFWriter pdfWriter;
                              pdfWriter.StartPDF(destinationPdfPath.toUtf8().toStdString(), ePDFVersionMax);
                              for (const QString &path : pdfFilePaths)
                              {
                                  pdfWriter.AppendPDFPagesFromPDF(path.toUtf8().toStdString(), PDFPageRange());
                                  if (std::unique_lock lock(taskMutex); stopped)
                                      return;
                                  emit signal_fileProcessed();
                              }
                              pdfWriter.EndPDF();
                          });
        }
        cv.notify_one();
    }
}

void AbstractPdfBuilder::loop()
{
    std::function<void()> fn;
    while(true)
    {
        {
            std::unique_lock lock(taskMutex);
            cv.wait(lock, [this]() -> bool
                    {
                        if (stopped)
                            return true;
                        return !tasks.empty();
                    });
            if (stopped)
                return;

            if (!tasks.empty())
            {
                fn = tasks.front();
                tasks.pop();
            }
        }
        try
        {
            fn();
        }
        catch(std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
        catch(...)
        {
            std::cerr << "unknown exception" << std::endl;
        }
    }
}

void AbstractPdfBuilder::stop()
{
    {
        std::unique_lock lk(taskMutex);
        stopped = true;
    }
    cv.notify_all();
}

void AbstractPdfBuilder::slot_cancelled()
{
    stop();
    emit signal_cancelled();
}

QString AbstractPdfBuilder::findTitleFileName(const QString &parentPath) const
{
    const QDir parentDir(parentPath);
    if (!parentDir.exists())
    {
        return QString();
    }
    const QFileInfoList pdfFiles = parentDir.entryInfoList(QStringList{"*.pdf"}, QDir::Files, QDir::Name);
    if (pdfFiles.isEmpty())
    {
        return QString();
    }
    auto it = std::find_if(pdfFiles.cbegin(), pdfFiles.cend(),
                           [](const QFileInfo &info) -> bool
                           {
                               return info.fileName().startsWith("Титул ");
                           });
    if (it == pdfFiles.cend())
    {
        return QString();
    }
    return (*it).fileName();
}
