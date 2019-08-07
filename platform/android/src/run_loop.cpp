#include "run_loop_impl.hpp"

#include <mbgl/util/platform.hpp>
#include <mbgl/util/thread_local.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/event.hpp>

#include <android/looper.h>

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <mbgl/util/logging.hpp>

#define PIPE_OUT 0
#define PIPE_IN  1

namespace {

using namespace mbgl::util;

int looperCallbackNew(int fd, int, void* data) {
    int buffer[1];
    while (read(fd, buffer, sizeof(buffer)) > 0) {}

    auto runLoopImpl = reinterpret_cast<RunLoop::Impl*>(data);

    runLoopImpl->coalesce.clear();
    ALooper_wake(runLoopImpl->loop);

    return 1;
}

int looperCallbackDefault(int fd, int, void* data) {
    int buffer[1];
    while (read(fd, buffer, sizeof(buffer)) > 0) {}

    auto runLoopImpl = reinterpret_cast<RunLoop::Impl*>(data);

    runLoopImpl->coalesce.clear();
    runLoopImpl->runLoop->runOnce();

    if (!runLoopImpl->running) {
        ALooper_wake(runLoopImpl->loop);
    }

    return 1;
}

} // namespace

namespace mbgl {
namespace util {

// This is needed only for the RunLoop living on the main thread because of
// how we implement timers. We sleep on `ALooper_pollAll` until the next
// timeout, but on the main thread `ALooper_pollAll` is called by the activity
// automatically, thus we cannot set the timeout. Instead we wake the loop
// with an external file descriptor event coming from this thread.
//
// Usually an actor should not carry pointers to other threads, but in
// this case the RunLoop itself owns the Alarm and calling wake() is the most
// efficient way of waking up the RunLoop and it is also thread-safe.
class Alarm {
public:
    Alarm(ActorRef<Alarm>, RunLoop::Impl* loop_) : loop(loop_) {}

    void set(const Milliseconds& timeout) {
        alarm.start(timeout, mbgl::Duration::zero(), [this]() { loop->wake(); });
    }

private:
    Timer alarm;
    RunLoop::Impl* loop;
};

RunLoop::Impl::Impl(RunLoop* runLoop_, RunLoop::Type type) : runLoop(runLoop_) {
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
            ALOOPER_EVENT_INPUT, looperCallbackNew, this);
        break;
    case Type::Default:
        ret = ALooper_addFd(loop, fds[PIPE_OUT], ALOOPER_POLL_CALLBACK,
            ALOOPER_EVENT_INPUT, looperCallbackDefault, this);
        alarm = std::make_unique<Thread<Alarm>>("Alarm", this);
        running = true;
        break;
    }

    if (ret != 1) {
        throw std::runtime_error("Failed to add file descriptor to Looper.");
    }
}

RunLoop::Impl::~Impl() {
    alarm.reset();

    if (ALooper_removeFd(loop, fds[PIPE_OUT]) != 1) {
        Log::Error(mbgl::Event::General, "Failed to remove file descriptor from Looper");
    }

    if (close(fds[PIPE_IN]) || close(fds[PIPE_OUT])) {
        Log::Error(mbgl::Event::General, "Failed to close file descriptor.");
    }

    ALooper_release(loop);
}

void RunLoop::Impl::wake() {
    if (coalesce.test_and_set(std::memory_order_acquire)) {
        return;
    }

    if (write(fds[PIPE_IN], "\n", 1) == -1) {
        throw std::runtime_error("Failed to write to file descriptor.");
    }
}

void RunLoop::Impl::addRunnable(Runnable* runnable) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        runnables.push_back(runnable);
    }

    wake();
}

void RunLoop::Impl::removeRunnable(Runnable* runnable) {
    std::lock_guard<std::mutex> lock(mutex);
    runnables.remove(runnable);
}

Milliseconds RunLoop::Impl::processRunnables() {
    auto now = Clock::now();
    auto nextDue = TimePoint::max();
    std::list<Runnable*> tmp;
    {
        std::lock_guard<std::mutex> lock(mutex);

        // O(N) but in the render thread where we get tons
        // of messages, the size of the list is usually 1~2.
        auto it = runnables.begin();
        while (it != runnables.end()) {
            Runnable* runnable = *it;

            auto const dueTime = runnable->dueTime();
            if (dueTime <= now) {
                tmp.push_back(runnable);
                runnables.erase(it++);
            } else {
                nextDue = std::min(nextDue, dueTime);
                ++it;
            }
        }
    }

    for (auto runnable : tmp) {
        runnable->runTask();
    }

    if (runnables.empty() || nextDue == TimePoint::max()) {
        return Milliseconds(-1);
    }

    auto timeout = std::chrono::duration_cast<Milliseconds>(nextDue - now);
    if (alarm) {
        alarm->actor().invoke(&Alarm::set, timeout);
    }

    return timeout;
}

RunLoop* RunLoop::Get() {
    assert(static_cast<RunLoop*>(Scheduler::GetCurrent()));
    return static_cast<RunLoop*>(Scheduler::GetCurrent());
}

RunLoop::RunLoop(Type type) : impl(std::make_unique<Impl>(this, type)) {
    Scheduler::SetCurrent(this);
}

RunLoop::~RunLoop() {
    Scheduler::SetCurrent(nullptr);
}

LOOP_HANDLE RunLoop::getLoopHandle() {
    return Get()->impl.get();
}

void RunLoop::wake() {
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
    invoke([&] {
        impl->running = false;
        impl->wake();
    });
}

void RunLoop::addWatch(int, Event, std::function<void(int, Event)>&&) {
    throw std::runtime_error("Not implemented.");
}

void RunLoop::removeWatch(int) {
    throw std::runtime_error("Not implemented.");
}

} // namespace util
} // namespace mbgl
