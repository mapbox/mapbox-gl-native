#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/scheduled_timer.hpp>

namespace mbgl {
    

ThreadPool::ThreadPool(std::size_t count) {
    std::promise<void> timerThreadRunning;
    
    timerThread = std::thread([&]() {
        platform::setCurrentThreadName("Timer thread");
        platform::makeThreadLowPriority();
        
        util::RunLoop loop(util::RunLoop::Type::New);
        timerLoop = &loop;
        timerThreadRunning.set_value();
        
        loop.run();
    });
    
    timerThreadRunning.get_future().get();
        
    threads.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        auto scheduler = this;
        threads.emplace_back([this, scheduler, i]() {
            Scheduler::SetCurrent(scheduler);
            platform::setCurrentThreadName(std::string{ "Worker " } + util::toString(i + 1));

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

    // Stop the threadpool threads
    cv.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }
    
    // Stop the timer thread
    timerLoop->stop();
    timerThread.join();
}

void ThreadPool::schedule(std::weak_ptr<Mailbox> mailbox) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(mailbox);
    }

    cv.notify_one();
}
    
    
std::unique_ptr<Scheduler::Scheduled> ThreadPool::schedule(Duration timeout, std::weak_ptr<Mailbox> mailbox, std::unique_ptr<Message> message) {
    return std::make_unique<ScheduledTimer>(*timerLoop, timeout, std::move(mailbox), std::move(message));
}

} // namespace mbgl
