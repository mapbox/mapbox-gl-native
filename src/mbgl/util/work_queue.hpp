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

class WorkQueue : private util::noncopyable {
public:
    WorkQueue();
    virtual ~WorkQueue();

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
