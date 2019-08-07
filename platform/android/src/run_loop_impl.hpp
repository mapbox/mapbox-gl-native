#pragma once

#include "jni.hpp"

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <atomic>
#include <list>
#include <memory>
#include <mutex>

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

private:
    friend RunLoop;

    int fds[2];

    std::unique_ptr<Thread<Alarm>> alarm;

    std::mutex mutex;
    std::list<Runnable*> runnables;
};

} // namespace util
} // namespace mbgl
