#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <atomic>
#include <list>
#include <memory>
#include <mutex>

struct ALooper;

namespace mbgl {
namespace util {

class RunLoop::Impl {
public:
    class Runnable {
    public:
        virtual ~Runnable() = default;

        virtual void runTask() = 0;
        virtual TimePoint dueTime() const = 0;

        std::list<Runnable*>::iterator iter;
    };

    void addRunnable(Runnable*);
    void removeRunnable(Runnable*);
    void initRunnable(Runnable*);

    Milliseconds processRunnables();

private:
    friend RunLoop;

    ALooper* loop = nullptr;
    std::atomic<bool> running;

    std::recursive_mutex mtx;
    std::list<Runnable*> runnables;
};

} // namespace util
} // namespace mbgl
