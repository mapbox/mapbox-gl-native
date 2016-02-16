#include <mbgl/util/timer.hpp>

#include <CoreFoundation/CoreFoundation.h>

namespace mbgl {
namespace util {

CFTimeInterval toCFTimeInterval(Duration duration) {
    return std::chrono::duration<CFTimeInterval>(duration).count();
}

CFAbsoluteTime toCFAbsoluteTime(Duration duration) {
    return CFAbsoluteTimeGetCurrent() + toCFTimeInterval(duration);
}

class Timer::Impl {
public:
    Impl(Duration timeout, Duration repeat, std::function<void()>&& fn)
        : task(std::move(fn)),
          loop(CFRunLoopGetCurrent()) {
        CFRunLoopTimerContext context = {
            0,
            this,
            nullptr,
            nullptr,
            nullptr
        };
        timer = CFRunLoopTimerCreate(kCFAllocatorDefault, toCFAbsoluteTime(timeout), toCFTimeInterval(repeat), 0, 0, perform, &context);
        CFRunLoopAddTimer(loop, timer, kCFRunLoopDefaultMode);
    }

    ~Impl() {
        CFRunLoopRemoveTimer(loop, timer, kCFRunLoopDefaultMode);
        CFRelease(timer);
    }

private:
    static void perform(CFRunLoopTimerRef, void* info) {
        reinterpret_cast<Impl*>(info)->task();
    }

    std::function<void()> task;

    CFRunLoopRef loop;
    CFRunLoopTimerRef timer;
};

Timer::Timer() = default;
Timer::~Timer() = default;

void Timer::start(Duration timeout, Duration repeat, std::function<void()>&& cb) {
    impl = std::make_unique<Impl>(timeout, repeat, std::move(cb));
}

void Timer::stop() {
    impl.reset();
}

} // namespace util
} // namespace mbgl
