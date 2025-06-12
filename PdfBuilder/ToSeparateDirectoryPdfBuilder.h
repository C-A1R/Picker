#ifndef TOSEPARATEDIRECTORYPDFBUILDER_H
#define TOSEPARATEDIRECTORYPDFBUILDER_H

#include "AbstractPdfBuilder.h"

/// сохраняет в указанный каталог
class ToSeparateDirectoryPdfBuilder : public AbstractPdfBuilder
{
    const QString separateDirectory;
public:
    ToSeparateDirectoryPdfBuilder(QString &&defenitFolder);
    ~ToSeparateDirectoryPdfBuilder() override = default;
protected:
    QString destinationFilePath(const QString &parentPath) override;
};

#endif // TOSEPARATEDIRECTORYPDFBUILDER_H
