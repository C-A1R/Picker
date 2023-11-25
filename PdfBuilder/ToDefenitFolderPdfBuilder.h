#ifndef TODEFENITFOLDERPDFBUILDER_H
#define TODEFENITFOLDERPDFBUILDER_H

#include "AbstractPdfBuilder.h"

/// сохраняет в указанный каталог
class ToDefenitFolderPdfBuilder : public AbstractPdfBuilder
{
    const QString defenitFolder;
public:
    ToDefenitFolderPdfBuilder(const QString &rootPath, QString &&defenitFolder);
    ~ToDefenitFolderPdfBuilder() override = default;
protected:
    QString destinationFilePath(const QString &parentPath) override;
};

#endif // TODEFENITFOLDERPDFBUILDER_H
