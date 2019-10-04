#include <mbgl/util/thread_pool.hpp>

#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/thread.hpp>

namespace mbgl {

ThreadPool::ThreadPool(std::size_t count) {
    threads.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        threads.emplace_back([this, i]() {
            platform::setCurrentThreadName(std::string{ "Worker " } + util::toString(i + 1));
            platform::attachThread();

            while (true) {
                std::unique_lock<std::mutex> lock(mutex);

                cv.wait(lock, [this] {
                    return !queue.empty() || terminate;
                });

                if (terminate) {
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
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        terminate = true;
    }

    cv.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }
}

void ThreadPool::schedule(std::function<void()> fn) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(std::move(fn));
    }

    cv.notify_one();
}

} // namespace mbgl
