#ifndef ABSTRACTPDFBUILDER_H
#define ABSTRACTPDFBUILDER_H

#include "IPdfBuilder.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

class ProjectItem;

class AbstractPdfBuilder : public IPdfBuilder
{
    Q_OBJECT

    std::mutex taskMutex;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    bool stopped = false;

    uint expectedProgress = 0;
    uint currentProgress = 0;

public:
    AbstractPdfBuilder();
    ~AbstractPdfBuilder() override;

protected:
    void exec(const QHash<QString, QStringList> &fileStructure) override;

private:
    void loop();
    void stop();
    void initProgressDlg();

signals:
    void signal_fileProcessed();
    void signal_allFilesProcessed();

private slots:
    void slot_cancelled();

protected:
    virtual QString destinationFilePath(const QString &parentPath) = 0;
    QString findTitleFileName(const QString &parentPath) const;
};


#endif // ABSTRACTPDFBUILDER_H
