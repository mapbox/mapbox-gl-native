#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/thread_local.hpp>

#include <CoreFoundation/CoreFoundation.h>

namespace mbgl {
namespace util {

static ThreadLocal<RunLoop>& current = *new ThreadLocal<RunLoop>;

class RunLoop::Impl {
public:
    std::unique_ptr<AsyncTask> async;
};

RunLoop* RunLoop::Get() {
    assert(current.get());
    return current.get();
}

RunLoop::RunLoop(Type)
  : impl(std::make_unique<Impl>()) {
    assert(!current.get());
    current.set(this);
    impl->async = std::make_unique<AsyncTask>(std::bind(&RunLoop::process, this));
}

RunLoop::~RunLoop() {
    current.set(nullptr);
}

void RunLoop::push(std::shared_ptr<WorkTask> task) {
    withMutex([&] { queue.push(std::move(task)); });
    impl->async->send();
}

void RunLoop::run() {
    CFRunLoopRun();
}

void RunLoop::runOnce() {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false);
}

void RunLoop::stop() {
    invoke([&] { CFRunLoopStop(CFRunLoopGetCurrent()); });
}

} // namespace util
} // namespace mbgl
