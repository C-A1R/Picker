#include "ToFoldersPdfBuilder.h"

#include <QDebug>
#include <QDir>

#include "PDFWriter/PDFWriter.h"

void ToFoldersPdfBuilder::exec(const QString &filename, const QStringList &paths) const
{
    if (filename.isEmpty() || paths.isEmpty())
    {
        return;
    }

    QDir dir{filename};
    if (dir.exists())
    {
        dir.remove(filename);
    }

    PDFWriter pdfWriter;
    pdfWriter.StartPDF(filename.toUtf8().toStdString(), ePDFVersionMax);
    for (const QString &path : paths)
    {
        qDebug() << "#" << path;
        pdfWriter.AppendPDFPagesFromPDF(path.toUtf8().toStdString(), PDFPageRange());
    }
    pdfWriter.EndPDF();
}
