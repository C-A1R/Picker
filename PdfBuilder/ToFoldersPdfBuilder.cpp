#include "ToFoldersPdfBuilder.h"
#include "PDFWriter/PDFWriter.h"

#include <QDir>

#include <thread>
#include <iostream>

ToFoldersPdfBuilder::ToFoldersPdfBuilder(const QString &rootPath)
    : rootPath{rootPath}
{
    threads.reserve(THREAD_COUNT);
    for (int i = 0 ; i < THREAD_COUNT; ++i)
    {
        threads.emplace_back(&ToFoldersPdfBuilder::loop, this);
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
        QString firstLevelPath = path.left(path.indexOf('/', rootPath.size() + 1));
        if (firstLevelPath == path)
        {
            firstLevelPath = rootPath;
        }
        structure[firstLevelPath] << path;
    }

    QHashIterator<QString, QStringList> iter(structure);
    while(iter.hasNext())
    {
        iter.next();
        const QStringList pdfFilePaths = iter.value();
        const QString resultFilePath = iter.key()
                                   + iter.key().right(iter.key().size() - iter.key().lastIndexOf('/'))
                                   + "_.pdf";
        {
            std::unique_lock lock(m);
            tasks.emplace([pdfFilePaths, resultFilePath]() -> void
                          {
                              QDir dir{resultFilePath};
                              if (dir.exists())
                              {
                                  dir.remove(resultFilePath);
                              }

                              PDFWriter pdfWriter;
                              pdfWriter.StartPDF(resultFilePath.toUtf8().toStdString(), ePDFVersionMax);
                              for (const QString &path : pdfFilePaths)
                              {
                                  pdfWriter.AppendPDFPagesFromPDF(path.toUtf8().toStdString(), PDFPageRange());
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
            std::unique_lock lock(m);
            cv.wait(lock, [this]() -> bool
                    {
                        return !tasks.empty();
                    });
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
