#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>
#include <android/log.h>

namespace mbgl {

ThreadPool::ThreadPool(std::size_t count) {
    threads.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        threads.emplace_back([this, i]() {
            platform::setCurrentThreadName(std::string{ "Worker " } + util::toString(i + 1));
            __android_log_write(ANDROID_LOG_ERROR, "JNI_THREAD", ("created new: " + mbgl::platform::getCurrentThreadName()).c_str());

            while (true) {
                std::unique_lock<std::mutex> lock(mutex);

                cv.wait(lock, [this] {
                    __android_log_write(ANDROID_LOG_ERROR, "JNI_THREAD", ("aweaken: " + mbgl::platform::getCurrentThreadName()).c_str());
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
