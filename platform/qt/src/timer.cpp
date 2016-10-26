#include "timer_impl.hpp"

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include <memory>

namespace mbgl {
namespace util {

Timer::Impl::Impl() {
#if QT_VERSION >= 0x050000
    timer.setTimerType(Qt::PreciseTimer);
#endif
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerFired()));
}

void Timer::Impl::start(uint64_t timeout, uint64_t repeat_, std::function<void ()>&& cb) {
    repeat = repeat_;
    callback = std::move(cb);

    timer.setSingleShot(true);
    timer.start(timeout);
}

void Timer::Impl::stop() {
    timer.stop();
}

void Timer::Impl::timerFired() {
    if (repeat) {
        timer.setSingleShot(false);
        timer.start(repeat);
    }

    callback();
}

Timer::Timer()
    : impl(std::make_unique<Impl>()) {
}

Timer::~Timer() = default;

void Timer::start(Duration timeout, Duration repeat, std::function<void()>&& cb) {
    impl->start(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count(),
                std::chrono::duration_cast<std::chrono::milliseconds>(repeat).count(),
                std::move(cb));
}

void Timer::stop() {
    impl->stop();
}

}
}
