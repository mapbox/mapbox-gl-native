#include <mbgl/util/run_loop.hpp>

namespace mbgl {
namespace util {

uv::tls<RunLoop> RunLoop::current;

RunLoop::RunLoop()
    : async(*loop, std::bind(&RunLoop::process, this)) {
}

void RunLoop::withMutex(std::function<void()>&& fn) {
    std::lock_guard<std::mutex> lock(mutex);
    fn();
}

void RunLoop::process() {
    Queue queue_;
    withMutex([&] { queue_.swap(queue); });

    while (!queue_.empty()) {
        (queue_.front())();
        queue_.pop();
    }
}

void RunLoop::run() {
    assert(!current.get());
    current.set(this);
    loop.run();
    current.set(nullptr);
}

void RunLoop::stop() {
    invoke([&] { async.unref(); });
}

}
}
