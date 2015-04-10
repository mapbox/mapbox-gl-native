#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/std.hpp>

#include <uv.h>

namespace /* anonymous */ {
inline void critical_section(std::mutex& mutex, std::function<void()> fn) {
    std::lock_guard<std::mutex> lock(mutex);
    fn();
}
}

namespace mbgl {
namespace util {

RunLoop::RunLoop()
    : runloop(util::make_unique<uv::loop>()),
      runloopAsync(util::make_unique<uv::async>(runloop->get(), std::bind(&RunLoop::process, this))) {
}

// Define here since we can't destroy the uv::* objects from just the header file.
RunLoop::~RunLoop() = default;

void RunLoop::start() {
    runloop->run();
}

void RunLoop::stop() {
    critical_section(runloopMutex, [this] { runloopQueue.push(nullptr); });
    runloopAsync->send();
}

void RunLoop::process() {
    std::queue<std::function<void()>> queue;
    critical_section(runloopMutex, [this, &queue] { queue.swap(runloopQueue); });
    while (!queue.empty()) {
        if (queue.front()) {
            queue.front()();
        } else {
            runloopAsync->unref();
        }
        queue.pop();
    }
}

void RunLoop::invoke(std::function<void()>&& fn) {
    if (fn) {
        critical_section(runloopMutex, [this, &fn] { runloopQueue.emplace(std::move(fn)); });
        runloopAsync->send();
    }
}

}
}
