#include "run_loop_impl.hpp"

#include <mbgl/util/thread_context.hpp>
#include <mbgl/util/thread_local.hpp>

#include <android/looper.h>

#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>

namespace {

using namespace mbgl::util;
static ThreadLocal<RunLoop>& current = *new ThreadLocal<RunLoop>;

} // namespace

namespace mbgl {
namespace util {

void RunLoop::Impl::addRunnable(Runnable* runnable) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (runnable->iter == runnables.end()) {
        auto iter = runnables.insert(runnables.end(), runnable);
        runnable->iter = std::move(iter);
    }

    ALooper_wake(loop);
}

void RunLoop::Impl::removeRunnable(Runnable* runnable) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (runnable->iter != runnables.end()) {
        runnables.erase(runnable->iter);
        runnable->iter = runnables.end();
    }
}

void RunLoop::Impl::initRunnable(Runnable* runnable) {
    runnable->iter = runnables.end();
}

Milliseconds RunLoop::Impl::processRunnables() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    auto now = Clock::now();
    auto nextDue = TimePoint::max();

    // O(N) but in the render thread where we get tons
    // of messages, the size of the list is usually 1~2.
    for (auto iter = runnables.begin(); iter != runnables.end();) {
        Runnable* runnable = *(iter++);

        auto const dueTime = runnable->dueTime();
        if (dueTime <= now) {
            runnable->runTask();
        } else {
            nextDue = std::min(nextDue, dueTime);
        }
    }

    if (runnables.empty() || nextDue == TimePoint::max()) {
        return Milliseconds(-1);
    } else {
        return std::chrono::duration_cast<Milliseconds>(nextDue - now);
    }
}

RunLoop* RunLoop::Get() {
    return current.get();
}

RunLoop::RunLoop(Type) : impl(std::make_unique<Impl>()) {
    using namespace mbgl::android;
    impl->detach = attach_jni_thread(theJVM, &impl->env, ThreadContext::getName());

    impl->loop = ALooper_prepare(0);
    assert(impl->loop);

    ALooper_acquire(impl->loop);

    current.set(this);
}

RunLoop::~RunLoop() {
    current.set(nullptr);

    ALooper_release(impl->loop);

    using namespace mbgl::android;
    detach_jni_thread(theJVM, &impl->env, impl->detach);
}

LOOP_HANDLE RunLoop::getLoopHandle() {
    return current.get()->impl.get();
}

void RunLoop::push(std::shared_ptr<WorkTask> task) {
    withMutex([&] { queue.push(std::move(task)); });
    ALooper_wake(impl->loop);
}

void RunLoop::run() {
    MBGL_VERIFY_THREAD(tid);

    impl->running = true;

    int outFd, outEvents;
    char *outData = nullptr;

    while (impl->running) {
        process();
        auto timeout = impl->processRunnables().count();
        ALooper_pollAll(timeout, &outFd, &outEvents, reinterpret_cast<void**>(&outData));
    }
}

void RunLoop::runOnce() {
    MBGL_VERIFY_THREAD(tid);

    int outFd, outEvents;
    char *outData = nullptr;

    process();
    impl->processRunnables().count();
    ALooper_pollOnce(0, &outFd, &outEvents, reinterpret_cast<void**>(&outData));
}

void RunLoop::stop() {
    impl->running = false;
    ALooper_wake(impl->loop);
}

void RunLoop::addWatch(int, Event, std::function<void(int, Event)>&&) {
    throw std::runtime_error("Not implemented.");
}

void RunLoop::removeWatch(int) {
    throw std::runtime_error("Not implemented.");
}

} // namespace util
} // namespace mbgl
