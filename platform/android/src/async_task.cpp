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
        : task(std::move(fn)) {
        loop->initRunnable(this);
    }

    ~Impl() {
        loop->removeRunnable(this);
    }

    void maySend() {
        if (!queued.test_and_set()) {
            loop->addRunnable(this);
        }
    }

    TimePoint dueTime() const override {
        return due;
    }

    void runTask() override {
        loop->removeRunnable(this);
        queued.clear();
        task();
    }

private:
    // Always expired, run immediately.
    const TimePoint due = Clock::now();

    RunLoop::Impl* loop = reinterpret_cast<RunLoop::Impl*>(RunLoop::getLoopHandle());

    std::atomic_flag queued = ATOMIC_FLAG_INIT;
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
