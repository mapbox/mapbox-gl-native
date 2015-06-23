#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/platform/platform.hpp>

#include <cassert>
#include <future>

namespace mbgl {

class Worker::Impl {
public:
    Impl(uv_loop_t*) {}

    void doWork(std::shared_ptr<WorkTask>& task) {
        task->runTask();
    }
};

Worker::Worker(std::size_t count) {
    util::ThreadContext context = {"Worker", util::ThreadType::Worker, util::ThreadPriority::Low};
    for (std::size_t i = 0; i < count; i++) {
        threads.emplace_back(std::make_unique<util::Thread<Impl>>(context));
    }
}

Worker::~Worker() = default;

std::unique_ptr<WorkRequest> Worker::send(Fn work, Fn after) {
    auto task = std::make_shared<WorkTask>(work, after);
    auto request = std::make_unique<WorkRequest>(task);

    threads[current]->invokeWithResult(&Worker::Impl::doWork, [task] {
        task->runAfter();
    }, task);

    current = (current + 1) % threads.size();
    return request;
}

} // end namespace mbgl
