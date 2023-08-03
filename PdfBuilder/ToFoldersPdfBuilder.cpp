#include "ToFoldersPdfBuilder.h"
#include "PDFWriter/PDFWriter.h"

#include <QDir>
#include <QProgressDialog>
#include <QThread>

#include <thread>
#include <iostream>

ToFoldersPdfBuilder::ToFoldersPdfBuilder(const QString &rootPath)
    : rootPath{rootPath}
{
    const unsigned int threads_count = 4/*std::thread::hardware_concurrency() - 1*/;
    threads.reserve(threads_count);
    for (unsigned int i = 0 ; i < threads_count; ++i)
    {
        threads.emplace_back(&ToFoldersPdfBuilder::loop, this);
    }
    progress.reset(new QProgressDialog("Сборка...", "Отмена", currentProgress, 0));
    progress->setWindowModality(Qt::ApplicationModal);
    connect(progress.get(), &QProgressDialog::canceled, this, &ToFoldersPdfBuilder::slot_cancelled);
    connect(this, &ToFoldersPdfBuilder::signal_fileProcessed, this, &ToFoldersPdfBuilder::slot_fileProcessed);
}

ToFoldersPdfBuilder::~ToFoldersPdfBuilder()
{
    stop();
    for (auto &t : threads)
    {
        t.join();
    }
}

void ToFoldersPdfBuilder::exec(const QStringList &paths)
{
    if (rootPath.isEmpty() || paths.isEmpty())
    {
        return;
    }

    QHash<QString, QStringList> structure;
    for (const QString &path : paths)
    {
        const QString parentPath = path.left(path.lastIndexOf('/'));
        structure[parentPath == path ? rootPath : parentPath] << path;
    }
    expectedProgress = paths.count();
    progress->setMaximum(paths.count());
    progress->show();
    QHashIterator<QString, QStringList> iter(structure);
    while(iter.hasNext())
    {
        iter.next();
        const QStringList pdfFilePaths = iter.value();
        const QString destinationPath = resultFilePath(iter.key());
        if (destinationPath.isEmpty())
        {
            continue;
        }
        {
            std::unique_lock lock(taskMutex);
            tasks.emplace([pdfFilePaths, destinationPath, this]() -> void
                          {
                              QDir dir{destinationPath};
                              if (dir.exists())
                              {
                                  dir.remove(destinationPath);
                              }

                              PDFWriter pdfWriter;
                              pdfWriter.StartPDF(destinationPath.toUtf8().toStdString(), ePDFVersionMax);
                              for (const QString &path : pdfFilePaths)
                              {
                                  pdfWriter.AppendPDFPagesFromPDF(path.toUtf8().toStdString(), PDFPageRange());
                                  if (std::unique_lock lock(taskMutex); stopped)
                                  {
                                      return;
                                  }
                                  emit signal_fileProcessed();
                              }
                              pdfWriter.EndPDF();
                          });
        }
        cv.notify_one();
    }
}

void ToFoldersPdfBuilder::loop()
{
    std::function<void()> fn;
    while(true)
    {
        {
            std::unique_lock lock(taskMutex);
            cv.wait(lock, [this]() -> bool
                    {
                        if (stopped)
                        {
                            return true;
                        }
                        return !tasks.empty();
                    });
            if (stopped)
            {
                return;
            }
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

void ToFoldersPdfBuilder::stop()
{
    {
        std::unique_lock lk(taskMutex);
        stopped = true;
    }
    cv.notify_all();
}

QString ToFoldersPdfBuilder::resultFilePath(const QString &destinationPath)
{
    const QDir dir(destinationPath);
    if (!dir.exists())
    {
        return QString();
    }
    const QFileInfoList pdfFiles = dir.entryInfoList(QStringList{"*.pdf"}, QDir::Files, QDir::Name);
    if (pdfFiles.isEmpty())
    {
        return QString();
    }
    auto it = std::find_if(pdfFiles.cbegin(), pdfFiles.cend(),
                           [](const QFileInfo &info) -> bool
                           {
                               return info.fileName().startsWith("Титул ");
                           });
    QString result;
    if (it != pdfFiles.cend())
    {
        result = destinationPath + '/' + (*it).fileName().remove("Титул ");
    }
    else
    {
        result = destinationPath
                 + destinationPath.right(destinationPath.size() - destinationPath.lastIndexOf('/'))
                 + ".pdf";
    }
    return result;
}

void ToFoldersPdfBuilder::slot_fileProcessed()
{
    progress->setValue(++currentProgress);
    if (currentProgress == expectedProgress)
    {
        emit signal_finished();
    }
}

void ToFoldersPdfBuilder::slot_cancelled()
{
    stop();
    emit signal_cancelled();
}
