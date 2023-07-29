#ifndef IPDFBUILDER_H
#define IPDFBUILDER_H

#include <QList>
#include <QtGlobal>

class IPdfBuilder
{
public:
    IPdfBuilder() = default;
    virtual ~IPdfBuilder() = default;

    virtual void build(const QList<quintptr> &orders) const = 0;
};

#endif // IPDFBUILDER_H
