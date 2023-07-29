#ifndef TOFOLDERSPDFBUILDER_H
#define TOFOLDERSPDFBUILDER_H

#include "IPdfBuilder.h"

class ToFoldersPdfBuilder : public IPdfBuilder
{
public:
    ToFoldersPdfBuilder() = default;
    ~ToFoldersPdfBuilder() override = default;

protected:
    void build(const QList<quintptr> &orders) const override;
};

#endif // TOFOLDERSPDFBUILDER_H
