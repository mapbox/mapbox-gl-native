#include <mbgl/util/worker.hpp>
#include <mbgl/platform/platform.hpp>

#include <cassert>
#include <future>

namespace mbgl {

class Worker::Impl {
public:
    Impl(uv_loop_t*) {}

    void doWork(std::packaged_task<void ()>& task) {
        task();
    }
};

Worker::Worker(std::size_t count) {
    for (std::size_t i = 0; i < count; i++) {
        threads.emplace_back(util::make_unique<util::Thread<Impl>>("Worker", util::ThreadPriority::Low));
    }
}

Worker::~Worker() = default;

WorkRequest Worker::send(Fn work, Fn after) {
    std::packaged_task<void ()> task(work);
    std::future<void> future = task.get_future();

    std::shared_ptr<std::atomic<bool>> joined = std::make_shared<std::atomic<bool>>();
    *joined = false;

    threads[current]->invokeWithResult(&Worker::Impl::doWork, [joined, after] {
        if (!*joined) {
            after();
        }
    }, task);

    current = (current + 1) % threads.size();
    return WorkRequest(std::move(future), joined);
}

}
