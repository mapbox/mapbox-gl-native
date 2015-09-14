#include <mbgl/util/work_queue.hpp>

#include <mbgl/util/run_loop.hpp>

namespace mbgl {
namespace util {

WorkQueue::WorkQueue() : runLoop(RunLoop::Get()) {
}

WorkQueue::~WorkQueue() {
    assert(runLoop == RunLoop::Get());

    // Cancel all pending WorkRequests.
    while (!queue.empty()) {
        queue.pop();
    }
}

void WorkQueue::push(std::function<void()>&& fn) {
    std::lock_guard<std::mutex> lock(queueMutex);

    auto workRequest = runLoop->invokeCancellable(std::bind(&WorkQueue::pop, this, std::move(fn)));
    queue.push(std::move(workRequest));
}

void WorkQueue::pop(const std::function<void()>& fn) {
    assert(runLoop == RunLoop::Get());

    fn();

    std::lock_guard<std::mutex> lock(queueMutex);
    queue.pop();
}

}
}
