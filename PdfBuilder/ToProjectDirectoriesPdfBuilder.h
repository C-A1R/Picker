#ifndef TOPROJECTDIRECTORIESPDFBUILDER_H
#define TOPROJECTDIRECTORIESPDFBUILDER_H

#include "AbstractPdfBuilder.h"

/// сохраняет в каталоги проекта
class ToProjectDirectoriesPdfBuilder : public AbstractPdfBuilder
{
public:
    ToProjectDirectoriesPdfBuilder(QStringList &&resultHolderPaths);
    ~ToProjectDirectoriesPdfBuilder() override = default;
protected:
    QString destinationFilePath(const QString &parentPath) override;
};

#endif // TOPROJECTDIRECTORIESPDFBUILDER_H
