#include <mbgl/util/worker.hpp>

#include <cassert>

namespace mbgl {

class Worker::Impl {
public:
    Impl(uv_loop_t*) {}

    void doWork(Fn work) {
        work();
    }
};

Worker::Worker(std::size_t count) {
    for (std::size_t i = 0; i < count; i++) {
        threads.emplace_back(util::make_unique<util::Thread<Impl>>("Worker"));
    }
}

Worker::~Worker() = default;

void Worker::send(Fn work, Fn after) {
    threads[current]->invokeWithResult(&Worker::Impl::doWork, after, work);
    current = (current + 1) % threads.size();
}

}
