#ifndef TOPARENTFOLDERSPDFBUILDER_H
#define TOPARENTFOLDERSPDFBUILDER_H

#include "AbstractPdfBuilder.h"

/// сохраняет в каталоги проекта
class ToParentFoldersPdfBuilder : public AbstractPdfBuilder
{
public:
    ToParentFoldersPdfBuilder(const QString &rootPath);
    ~ToParentFoldersPdfBuilder() override = default;
protected:
    QString destinationFilePath(const QString &parentPath) override;
};

#endif // TOPARENTFOLDERSPDFBUILDER_H
