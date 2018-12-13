#pragma once

#include <mbgl/actor/scheduler.hpp>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace mbgl {

class ThreadPool : public Scheduler {
public:
    ThreadPool(std::size_t count);
    ~ThreadPool() override;

    void schedule(std::weak_ptr<Mailbox>) override;

private:
    std::vector<std::thread> threads;
    std::queue<std::weak_ptr<Mailbox>> queue;
    std::mutex mutex;
    std::condition_variable cv;
    bool terminate { false };
};

} // namespace mbgl
