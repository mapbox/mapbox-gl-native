#ifndef MBGL_UTIL_WORKER
#define MBGL_UTIL_WORKER

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/async_queue.hpp>
#include <mbgl/util/channel.hpp>
#include <mbgl/util/util.hpp>

#include <thread>
#include <functional>

namespace mbgl {

class Worker : public mbgl::util::noncopyable {
public:
    using Fn = std::function<void ()>;

    Worker(uv_loop_t* loop, std::size_t count);
    ~Worker();

    void send(Fn work, Fn after);

private:
    void workLoop();
    void afterWork(Fn after);

    struct Work {
        Fn work;
        Fn after;
    };

    using Queue = util::AsyncQueue<std::function<void ()>>;

    std::size_t active = 0;
    Queue* queue = nullptr;
    Channel<Work> channel;
    std::vector<std::thread> threads;

    MBGL_STORE_THREAD(tid)
};

}

#endif
