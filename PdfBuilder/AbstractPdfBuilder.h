#ifndef ABSTRACTPDFBUILDER_H
#define ABSTRACTPDFBUILDER_H

#include "IPdfBuilder.h"

#include <QProgressDialog>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

class AbstractPdfBuilder : public IPdfBuilder
{
    Q_OBJECT

    const QString rootPath;
    std::mutex taskMutex;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    bool stopped = false;

    QScopedPointer<QProgressDialog> progress;
    uint expectedProgress = 0;
    uint currentProgress = 0;

public:
    AbstractPdfBuilder(const QString &rootPath);
    ~AbstractPdfBuilder() override;

protected:
    void exec(const QStringList &paths) override;

private:
    void loop();
    void stop();
    void initProgressDlg();

signals:
    void signal_fileProcessed();

private slots:
    void slot_fileProcessed();
    void slot_cancelled();

protected:
    virtual QString destinationFilePath(const QString &parentPath) = 0;
    QString findTitleFileName(const QString &parentPath) const;
};


#endif // ABSTRACTPDFBUILDER_H