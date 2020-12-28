#include "run_loop_impl.hpp"

#include <mbgl/util/async_task.hpp>
#include <mbgl/util/run_loop.hpp>

#include <atomic>
#include <functional>

namespace mbgl {
namespace util {

class AsyncTask::Impl : public RunLoop::Impl::Runnable {
public:
    Impl(std::function<void()>&& fn)
        : queued(true), task(std::move(fn)) {
    }

    ~Impl() {
        queued = true;
        loop->removeRunnable(this);
    }

    void maySend() {
        if (queued) {
            queued = false;
            loop->addRunnable(this);
        }
    }

    TimePoint dueTime() const override {
        return due;
    }

    void runTask() override {
        if (!queued) {
            queued = true;
            loop->removeRunnable(this);
            task();
        }
    }

private:
    // Always expired, run immediately.
    const TimePoint due = Clock::now();

    RunLoop::Impl* loop = reinterpret_cast<RunLoop::Impl*>(RunLoop::getLoopHandle());

    // TODO: Use std::atomic_flag if we ever drop
    // support for ARMv5
    std::atomic<bool> queued;
    std::function<void()> task;
};

AsyncTask::AsyncTask(std::function<void()>&& fn)
    : impl(std::make_unique<Impl>(std::move(fn))) {
}

AsyncTask::~AsyncTask() = default;

void AsyncTask::send() {
    impl->maySend();
}

} // namespace util
} // namespace mbgl
