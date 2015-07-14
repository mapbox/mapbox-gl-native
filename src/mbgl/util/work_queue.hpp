#ifndef MBGL_UTIL_WORK_QUEUE
#define MBGL_UTIL_WORK_QUEUE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/work_request.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace mbgl {
namespace util {

class RunLoop;

// The WorkQueue will manage a queue of closures
// and it will make sure they get executed on the
// thread that created the WorkQueue. All pending
// works are canceled when the queue gets destructed.
class WorkQueue : private util::noncopyable {
public:
    WorkQueue();
    ~WorkQueue();

    // Push a closure to the queue. It is advised to
    // only push tasks calling functions on the object
    // that owns the queue to avoid use after free errors.
    void push(std::function<void()>&&);

private:
    void pop(const std::function<void()>&);

    std::queue<std::unique_ptr<WorkRequest>> queue;
    std::mutex queueMutex;

    RunLoop* runLoop;
};

}
}

#endif
