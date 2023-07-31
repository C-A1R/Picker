#ifndef TOFOLDERSPDFBUILDER_H
#define TOFOLDERSPDFBUILDER_H

#include "IPdfBuilder.h"

#include <QProgressDialog>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

class ToFoldersPdfBuilder : public IPdfBuilder
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
    ToFoldersPdfBuilder(const QString &rootPath);
    ~ToFoldersPdfBuilder() override;

protected:
    void exec(const QStringList &paths) override;

private:
    void loop();
    void stop();
    QString resultFilePath(const QString &firstLevelPath);

signals:
    void signal_fileProcessed();

private slots:
    void slot_fileProcessed();
    void slot_cancelled();
};

#endif // TOFOLDERSPDFBUILDER_H
