#ifndef TOPARENTANDDEFENITFOLDERPDFBUILDER_H
#define TOPARENTANDDEFENITFOLDERPDFBUILDER_H

#include "ToParentFoldersPdfBuilder.h"

class ToParentAndDefenitFolderPdfBuilder : public ToParentFoldersPdfBuilder
{
    QString defenitFolder;
    QList<QString> destinations;
public:
    ToParentAndDefenitFolderPdfBuilder(const QString &rootPath, QString &&defenitFolder);
    ~ToParentAndDefenitFolderPdfBuilder() override = default;
protected:
    void exec(const QStringList &paths) override;
    QString destinationFilePath(const QString &parentPath) override;
};

#endif // TOPARENTANDDEFENITFOLDERPDFBUILDER_H
