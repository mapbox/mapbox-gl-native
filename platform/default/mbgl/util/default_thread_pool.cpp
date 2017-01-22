#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/util/platform.hpp>

namespace mbgl {

ThreadPool::ThreadPool(std::size_t count, const util::ThreadContext& context) {
    threads.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        threads.emplace_back([this, context] () {
            platform::setCurrentThreadName(context.name);
            if (context.priority == util::ThreadPriority::Low) {
                platform::makeThreadLowPriority();
            }

            while (true) {
                std::unique_lock<std::mutex> lock(mutex);

                cv.wait(lock, [this] {
                    return !queue.empty() || terminate;
                });

                if (terminate) {
                    return;
                }

                auto mailbox = queue.front();
                queue.pop();
                lock.unlock();

                Mailbox::maybeReceive(mailbox);
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

void ThreadPool::schedule(std::weak_ptr<Mailbox> mailbox) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(mailbox);
    }

    cv.notify_one();
}

} // namespace mbgl
