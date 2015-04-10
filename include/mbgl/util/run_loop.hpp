#ifndef MBGL_UTIL_RUN_LOOP
#define MBGL_UTIL_RUN_LOOP

#include <mutex>
#include <functional>
#include <queue>

namespace uv {
class async;
class loop;
}

namespace mbgl {
namespace util {

template <typename T> class Thread;

class RunLoop {
    friend Thread<RunLoop>;

protected:
    // These are called by the Thread<> wrapper.
    RunLoop();
    ~RunLoop();

    // Called by the Thread<> wrapper to start the loop. When you implement this
    // method in a child class, you *must* call this function as the last action.
    void start();

protected:
    // Called by the Thread<> wrapper to terminate this loop.
    void stop();

private:
    // Invokes function in the run loop.
    void process();

public:
    // Schedules a function to be executed as part of this run loop.
    void invoke(std::function<void()> fn);

private:
    const std::unique_ptr<uv::loop> runloop;
    const std::unique_ptr<uv::async> runloopAsync;
    std::mutex runloopMutex;
    std::queue<std::function<void()>> runloopQueue;
};

}
}

#endif