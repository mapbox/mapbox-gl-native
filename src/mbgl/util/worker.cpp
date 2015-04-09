#include <mbgl/util/worker.hpp>

#include <cassert>

namespace mbgl {

Worker::Worker(uv_loop_t* loop, std::size_t count)
    : queue(new Queue(loop, [this](Fn after) { afterWork(after); }))
{
    queue->unref();

    for (std::size_t i = 0; i < count; i++) {
        threads.emplace_back(&Worker::workLoop, this);
    }
}

Worker::~Worker() {
    MBGL_VERIFY_THREAD(tid);

    if (active++ == 0) {
        queue->ref();
    }

    channel.send(Work());

    for (auto& thread : threads) {
        thread.join();
    }

    queue->stop();
}

void Worker::send(Fn work, Fn after) {
    MBGL_VERIFY_THREAD(tid);
    assert(work);

    if (active++ == 0) {
        queue->ref();
    }

    channel.send({work, after});
}

void Worker::workLoop() {
#ifdef __APPLE__
    pthread_setname_np("Worker");
#endif

    while (true) {
        Work item = channel.receive();

        if (!item.work)
            break;

        item.work();
        queue->send(std::move(item.after));
    }

    // Make sure to close all other workers too.
    channel.send(Work());
}

void Worker::afterWork(Fn after) {
    if (after) {
        after();
    }

    if (--active == 0) {
        queue->unref();
    }
}

}
