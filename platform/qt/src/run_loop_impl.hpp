#pragma once

#include <mbgl/util/logging.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/run_loop.hpp>

#include <QEventLoop>
#include <QObject>
#include <QSocketNotifier>

#include <unordered_map>

namespace mbgl {
namespace util {

using WatchCallback = std::function<void(int, RunLoop::Event)>;
using WatchPair = std::pair<std::unique_ptr<QSocketNotifier>, WatchCallback>;

class RunLoop::Impl : public QObject {
    Q_OBJECT

public:
    Impl() = default;

    RunLoop::Type type;

    std::unique_ptr<QEventLoop> loop;
    std::unique_ptr<AsyncTask> async;

    std::unordered_map<int, WatchPair> readPoll;
    std::unordered_map<int, WatchPair> writePoll;

public slots:
    void onReadEvent(int fd);
    void onWriteEvent(int fd);
};

} // namespace util
} // namespace mbgl
