#ifndef TOFOLDERSPDFBUILDER_H
#define TOFOLDERSPDFBUILDER_H

#include "IPdfBuilder.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

class ToFoldersPdfBuilder : public IPdfBuilder
{
    const QString rootPath;

    static const ushort THREAD_COUNT = 4;
    std::mutex m;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

public:
    ToFoldersPdfBuilder(const QString &rootPath);
    ~ToFoldersPdfBuilder() override = default;

protected:
    void exec(const QStringList &paths) override;

private:
    void loop();
};

#endif // TOFOLDERSPDFBUILDER_H
