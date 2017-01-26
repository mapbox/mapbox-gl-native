#include "run_loop_impl.hpp"

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include <functional>

namespace mbgl {
namespace util {

class Timer::Impl : public RunLoop::Impl::Runnable {
public:
    Impl() {
        loop->initRunnable(this);
    }

    ~Impl() {
        stop();
    }

    void start(Duration timeout, Duration repeat_, std::function<void ()>&& task_) {
        stop();

        repeat = repeat_;
        task = std::move(task_);
        // Prevent overflows when timeout is set to Duration::max()
        due = (timeout == Duration::max()) ? std::chrono::time_point<Clock>::max() : Clock::now() + timeout;
        loop->addRunnable(this);
    }

    void stop() {
        loop->removeRunnable(this);
    }

    void reschedule() {
        if (repeat != Duration::zero()) {
            due = Clock::now() + repeat;
            loop->wake();
        } else {
            stop();
        }
    }

    TimePoint dueTime() const override {
        return due;
    }

    void runTask() override {
        reschedule();
        task();
    }

private:
    TimePoint due;
    Duration repeat;

    RunLoop::Impl* loop = reinterpret_cast<RunLoop::Impl*>(RunLoop::getLoopHandle());

    std::function<void()> task;
};

Timer::Timer()
    : impl(std::make_unique<Impl>()) {
}

Timer::~Timer() = default;

void Timer::start(Duration timeout, Duration repeat, std::function<void()>&& cb) {
    impl->start(timeout, repeat, std::move(cb));
}

void Timer::stop() {
    impl->stop();
}

} // namespace util
} // namespace mbgl
