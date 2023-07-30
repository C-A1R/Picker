#ifndef TOFOLDERSPDFBUILDER_H
#define TOFOLDERSPDFBUILDER_H

#include "IPdfBuilder.h"

class ToFoldersPdfBuilder : public IPdfBuilder
{
public:
    ToFoldersPdfBuilder() = default;
    ~ToFoldersPdfBuilder() override = default;

protected:
    void exec(const QString &filename, const QStringList &paths) const override;
};

#endif // TOFOLDERSPDFBUILDER_H
