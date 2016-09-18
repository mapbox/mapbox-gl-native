#include <mbgl/actor/thread_pool.hpp>
#include <mbgl/actor/mailbox.hpp>

namespace mbgl {

ThreadPool::ThreadPool(std::size_t count) {
    threads.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        threads.emplace_back([this] () {
            while (true) {
                std::unique_lock<std::mutex> lock(mutex);

                cv.wait(lock, [this] {
                    return !queue.empty() || terminate.load();
                });

                if (terminate.load()) {
                    return;
                }

                auto mailbox = queue.front();
                queue.pop();
                lock.unlock();

                if (auto locked = mailbox.lock()) {
                    locked->receive();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    terminate.store(true);
    cv.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }
}

void ThreadPool::schedule(std::weak_ptr<Mailbox> mailbox) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(mailbox);
    cv.notify_one();
}

} // namespace mbgl
