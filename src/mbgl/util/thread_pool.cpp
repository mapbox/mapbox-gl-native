#include <mbgl/util/thread_pool.hpp>

#include <mbgl/platform/settings.hpp>
#include <mbgl/platform/thread.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {

ThreadedSchedulerBase::~ThreadedSchedulerBase() = default;

void ThreadedSchedulerBase::terminate() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        terminated = true;
    }
    cv.notify_all();
}

std::thread ThreadedSchedulerBase::makeSchedulerThread(size_t index) {
    return std::thread([this, index] {
        auto& settings = platform::Settings::getInstance();
        auto value = settings.get(platform::EXPERIMENTAL_THREAD_PRIORITY_WORKER);
        if (auto* priority = value.getDouble()) {
            platform::setCurrentThreadPriority(*priority);
        }

        platform::setCurrentThreadName(std::string{"Worker "} + util::toString(index + 1));
        platform::attachThread();

        while (true) {
            std::unique_lock<std::mutex> lock(mutex);

            cv.wait(lock, [this] { return !queue.empty() || terminated; });

            if (terminated) {
                platform::detachThread();
                return;
            }

            auto function = std::move(queue.front());
            queue.pop();
            lock.unlock();
            if (function) function();
        }
    });
}

void ThreadedSchedulerBase::schedule(std::function<void()> fn) {
    assert(fn);
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(std::move(fn));
    }

    cv.notify_one();
}

} // namespace mbgl
