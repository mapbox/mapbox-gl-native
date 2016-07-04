#pragma once

#include <mbgl/util/async_task.hpp>

#include <QObject>

#include <functional>
#include <atomic>

namespace mbgl {
namespace util {

class RunLoop;

class AsyncTask::Impl : public QObject {
    Q_OBJECT

public:
    Impl(std::function<void()> &&);

    void maySend();

public slots:
    void runTask();

signals:
    void send();

private:
    RunLoop* runLoop;

    std::function<void()> task;
    std::atomic_flag queued = ATOMIC_FLAG_INIT;
};


} // namespace util
} // namespace mbgl
