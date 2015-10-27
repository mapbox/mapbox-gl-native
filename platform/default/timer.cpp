#include <mbgl/util/timer.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/uv_detail.hpp>

namespace mbgl {
namespace util {

class Timer::Impl {
public:
    Impl()
        : timer(reinterpret_cast<uv_loop_t*>(RunLoop::getLoopHandle())) {
    }

    uv::timer timer;
};

Timer::Timer()
    : impl(std::make_unique<Impl>()) {
}

Timer::~Timer() {
}

void Timer::start(Duration timeout, Duration repeat, std::function<void()>&& cb) {
    impl->timer.start(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count(),
                      std::chrono::duration_cast<std::chrono::milliseconds>(repeat).count(),
                      std::move(cb));
}

void Timer::stop() {
    impl->timer.stop();
}

void Timer::unref() {
    impl->timer.unref();
}

}
}
