#include <mbgl/util/async_task.hpp>

#include <CoreFoundation/CoreFoundation.h>

#include <atomic>

namespace mbgl {
namespace util {

class AsyncTask::Impl {
public:
    Impl(std::function<void()>&& fn)
        : task(std::move(fn)),
          loop(CFRunLoopGetCurrent()) {
        CFRunLoopSourceContext context = {
            0,
            this,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            perform
        };
        source = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &context);
        CFRunLoopAddSource(loop, source, kCFRunLoopCommonModes);
    }

    ~Impl() {
        CFRunLoopSourceInvalidate(source);
        CFRelease(source);
    }

    void maySend() {
        if (!queued.test_and_set()) {
            CFRunLoopSourceSignal(source);
            CFRunLoopWakeUp(loop);
        }
    }

    void runTask() {
        queued.clear();
        task();
    }

private:
    static void perform(void* info) {
        reinterpret_cast<Impl*>(info)->runTask();
    }

    std::function<void()> task;
    std::atomic_flag queued = ATOMIC_FLAG_INIT;

    CFRunLoopRef loop;
    CFRunLoopSourceRef source;
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
