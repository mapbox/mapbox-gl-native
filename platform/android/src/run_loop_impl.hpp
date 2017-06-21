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

        std::list<Runnable*>::iterator iter;
    };

    Impl(RunLoop*, RunLoop::Type);
    ~Impl();

    void wake();

    void addRunnable(Runnable*);
    void removeRunnable(Runnable*);
    void initRunnable(Runnable*);

    Milliseconds processRunnables();

    ALooper* loop = nullptr;
    RunLoop* runLoop = nullptr;
    std::atomic<bool> running;

private:
    friend RunLoop;

    int fds[2];

    JNIEnv *env = nullptr;
    bool detach = false;

    std::unique_ptr<Thread<Alarm>> alarm;

    std::recursive_mutex mtx;
    std::list<Runnable*> runnables;
    std::list<Runnable*>::iterator nextRunnable;
};

} // namespace util
} // namespace mbgl
