#pragma once

#include "jni.hpp"

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>

struct ALooper;

namespace mbgl {
namespace util {

template <typename T> class Thread;
class Alarm;

class RunLoop::Impl {
public:
    class Runnable {
    public:
        virtual ~Runnable() = default;

        virtual void runTask() = 0;
        virtual TimePoint dueTime() const = 0;
    };

    Impl(RunLoop*, RunLoop::Type);
    ~Impl();

    void wake();

    void addRunnable(Runnable*);
    void removeRunnable(Runnable*);

    Milliseconds processRunnables();

    ALooper* loop = nullptr;
    RunLoop* runLoop = nullptr;
    std::atomic<bool> running;
    std::atomic_flag coalesce = ATOMIC_FLAG_INIT;

#ifdef MBGL_TEST
    using WatchCallback = std::function<void(int, RunLoop::Event)>;
    std::unordered_map<int, WatchCallback> readPoll;
#endif

private:
    friend RunLoop;

    int fds[2];

    std::unique_ptr<Thread<Alarm>> alarm;

    std::mutex mutex;
    std::list<Runnable*> runnables;
};

} // namespace util
} // namespace mbgl
