#ifndef TOPROJECTANDSEPARATEDIRECTORIESPDFBUILDER_H
#define TOPROJECTANDSEPARATEDIRECTORIESPDFBUILDER_H

#include "ToProjectDirectoriesPdfBuilder.h"

/// сохраняет в указанный каталог + каталоги проекта
class ToProjectAndSeparateDirectoryPdfBuilder : public ToProjectDirectoriesPdfBuilder
{
    QString defenitFolder;
    QList<QString> destinations;
public:
    ToProjectAndSeparateDirectoryPdfBuilder(QString &&defenitFolder);
    ~ToProjectAndSeparateDirectoryPdfBuilder() override = default;
protected:
    void exec(const QHash<QString, QStringList> &fileStructure) override;
    QString destinationFilePath(const QString &parentPath) override;
private slots:
    void slot_allFilesProcessed();
};

#endif // TOPROJECTANDSEPARATEDIRECTORIESPDFBUILDER_H
