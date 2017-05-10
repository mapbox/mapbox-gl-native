#include <mbgl/util/timer.hpp>

#include <mbgl/util/run_loop.hpp>

#include <uv.h>

namespace mbgl {
namespace util {

class Timer::Impl {
public:
    Impl() : timer(new uv_timer_t) {
        auto* loop = reinterpret_cast<uv_loop_t*>(RunLoop::getLoopHandle());
        if (uv_timer_init(loop, timer) != 0) {
            throw std::runtime_error("Failed to initialize timer.");
        }

        handle()->data = this;
        uv_unref(handle());
    }

    ~Impl() {
        uv_close(handle(), [](uv_handle_t* h) {
            delete reinterpret_cast<uv_timer_t*>(h);
        });
    }

    void start(uint64_t timeout, uint64_t repeat, std::function<void ()>&& cb_) {
        cb = std::move(cb_);
        if (uv_timer_start(timer, timerCallback, timeout, repeat) != 0) {
            throw std::runtime_error("Failed to start timer.");
        }
    }

    void stop() {
        cb = nullptr;
        if (uv_timer_stop(timer) != 0) {
            throw std::runtime_error("Failed to stop timer.");
        }
    }

private:
    static void timerCallback(uv_timer_t* handle) {
        reinterpret_cast<Impl*>(handle->data)->cb();
    }

    uv_handle_t* handle() {
        return reinterpret_cast<uv_handle_t*>(timer);
    }

    uv_timer_t* timer;

    std::function<void()> cb;
};

Timer::Timer()
    : impl(std::make_unique<Impl>()) {
}

Timer::~Timer() = default;

void Timer::start(Duration timeout, Duration repeat, std::function<void()>&& cb) {
    impl->start(std::chrono::duration_cast<Milliseconds>(timeout).count(),
                std::chrono::duration_cast<Milliseconds>(repeat).count(),
                std::move(cb));
}

void Timer::stop() {
    impl->stop();
}

} // namespace util
} // namespace mbgl
