#include "run_loop_impl.hpp"

#include <mbgl/util/thread_context.hpp>
#include <mbgl/util/thread_local.hpp>

#include <android/looper.h>

#include <algorithm>
#include <cassert>
#include <functional>
#include <stdexcept>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

#define PIPE_OUT 0
#define PIPE_IN  1

namespace {

using namespace mbgl::util;
static ThreadLocal<RunLoop>& current = *new ThreadLocal<RunLoop>;

int looperCallbackNew(int fd, int, void* data) {
    int buffer[1];
    while (read(fd, buffer, sizeof(buffer)) > 0) {}

    auto loop = reinterpret_cast<ALooper*>(data);
    ALooper_wake(loop);

    return 1;
}

int looperCallbackDefault(int fd, int, void* data) {
    int buffer[1];
    while (read(fd, buffer, sizeof(buffer)) > 0) {}

    auto runLoopImpl = reinterpret_cast<RunLoop::Impl*>(data);
    auto runLoop = runLoopImpl->runLoop;

    runLoop->runOnce();

    if (!runLoopImpl->running) {
        ALooper_wake(runLoopImpl->loop);
    }

    return 1;
}

} // namespace

namespace mbgl {
namespace util {

RunLoop::Impl::Impl(RunLoop* runLoop_, RunLoop::Type type) : runLoop(runLoop_) {
    using namespace mbgl::android;
    detach = attach_jni_thread(theJVM, &env, "");

    loop = ALooper_prepare(0);
    assert(loop);

    ALooper_acquire(loop);

    if (pipe(fds)) {
        throw std::runtime_error("Failed to create pipe.");
    }

    if (fcntl(fds[PIPE_OUT], F_SETFL, O_NONBLOCK)) {
        throw std::runtime_error("Failed to set pipe read end non-blocking.");
    }

    int ret = 0;

    switch (type) {
    case Type::New:
        ret = ALooper_addFd(loop, fds[PIPE_OUT], ALOOPER_POLL_CALLBACK,
            ALOOPER_EVENT_INPUT, looperCallbackNew, loop);
        break;
    case Type::Default:
        ret = ALooper_addFd(loop, fds[PIPE_OUT], ALOOPER_POLL_CALLBACK,
            ALOOPER_EVENT_INPUT, looperCallbackDefault, this);
        break;
    }

    if (ret != 1) {
        throw std::runtime_error("Failed to add file descriptor to Looper.");
    }
}

RunLoop::Impl::~Impl() {
    if (ALooper_removeFd(loop, fds[PIPE_OUT]) != 1) {
        throw std::runtime_error("Failed to remove file descriptor from Looper.");
    }

    if (close(fds[PIPE_IN]) || close(fds[PIPE_OUT])) {
        throw std::runtime_error("Failed to close file descriptor.");
    }

    ALooper_release(loop);

    using namespace mbgl::android;
    detach_jni_thread(theJVM, &env, detach);
}

void RunLoop::Impl::wake() {
    if (write(fds[PIPE_IN], "\n", 1) == -1) {
        throw std::runtime_error("Failed to write to file descriptor.");
    }
}

void RunLoop::Impl::addRunnable(Runnable* runnable) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (runnable->iter == runnables.end()) {
        auto iter = runnables.insert(runnables.end(), runnable);
        runnable->iter = std::move(iter);
    }

    wake();
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

RunLoop::RunLoop(Type type) : impl(std::make_unique<Impl>(this, type)) {
    current.set(this);
}

RunLoop::~RunLoop() {
    current.set(nullptr);
}

LOOP_HANDLE RunLoop::getLoopHandle() {
    return current.get()->impl.get();
}

void RunLoop::push(std::shared_ptr<WorkTask> task) {
    withMutex([&] { queue.push(std::move(task)); });
    impl->wake();
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

    process();
    impl->processRunnables();
}

void RunLoop::stop() {
    impl->running = false;
    impl->wake();
}

void RunLoop::addWatch(int, Event, std::function<void(int, Event)>&&) {
    throw std::runtime_error("Not implemented.");
}

void RunLoop::removeWatch(int) {
    throw std::runtime_error("Not implemented.");
}

} // namespace util
} // namespace mbgl
