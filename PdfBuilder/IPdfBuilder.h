#ifndef IPDFBUILDER_H
#define IPDFBUILDER_H

#include <QObject>
#include <QString>

class IPdfBuilder : public QObject
{
    Q_OBJECT
public:
    IPdfBuilder() = default;
    virtual ~IPdfBuilder() = default;
    virtual void exec(const QHash<QString, QStringList> &fileStructure) = 0;
signals:
    void signal_finished();
    void signal_cancelled();
};

#endif // IPDFBUILDER_H
