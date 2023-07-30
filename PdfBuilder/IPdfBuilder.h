#ifndef IPDFBUILDER_H
#define IPDFBUILDER_H

#include <QString>

class IPdfBuilder
{
public:
    IPdfBuilder() = default;
    virtual ~IPdfBuilder() = default;

    virtual void exec(const QString &filename, const QStringList &paths) const = 0;
};

#endif // IPDFBUILDER_H
