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
        const QString firstLevelPath = path.left(path.indexOf('/', rootPath.size() + 1));
        structure[firstLevelPath == path ? rootPath : firstLevelPath] << path;
    }

    QHashIterator<QString, QStringList> iter(structure);
    while(iter.hasNext())
    {
        iter.next();       
        const QStringList pdfFilePaths = iter.value();
        const QString resultPath = resultFilePath(iter.key());
        if (resultPath.isEmpty())
        {
            continue;
        }
        {
            std::unique_lock lock(m);
            tasks.emplace([pdfFilePaths, resultPath]() -> void
                          {
                              QDir dir{resultPath};
                              if (dir.exists())
                              {
                                  dir.remove(resultPath);
                              }

                              PDFWriter pdfWriter;
                              pdfWriter.StartPDF(resultPath.toUtf8().toStdString(), ePDFVersionMax);
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

QString ToFoldersPdfBuilder::resultFilePath(const QString &firstLevelPath)
{
    const QDir dir(firstLevelPath);
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
        result = firstLevelPath + '/' + (*it).fileName().remove("Титул ");
    }
    else
    {
        result = firstLevelPath
                 + firstLevelPath.right(firstLevelPath.size() - firstLevelPath.lastIndexOf('/'))
                 + ".pdf";
    }
    return result;
}
