#pragma once

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>

namespace mbgl {

struct ThreadLifecycle {
    using ThreadData = void*;

    ThreadLifecycle() = default;
    ThreadLifecycle(std::function<ThreadData()> onThreadCreated_,
                    std::function<void(ThreadData)> onThreadDestroyed_) :
            onThreadCreatedFn(onThreadCreated_),
            onThreadDestroyedFn(onThreadDestroyed_) {}

    void onThreadCreated() const {
        threadData = onThreadCreatedFn();
    }

    void onThreadDestroyed() const {
        onThreadDestroyedFn(threadData);
    }

private:
    mutable ThreadData threadData;
    std::function<ThreadData()> onThreadCreatedFn = [] { return nullptr; };
    std::function<void(ThreadData)> onThreadDestroyedFn = [](ThreadData) {};
};

class ThreadPool final : public Scheduler {
public:
    explicit ThreadPool(std::size_t count, ThreadLifecycle _lifecycle = ThreadLifecycle());
    ~ThreadPool() override;

    void schedule(std::weak_ptr<Mailbox>) override;

private:
    std::vector<std::thread> threads;
    std::queue<std::weak_ptr<Mailbox>> queue;
    std::mutex mutex;
    std::condition_variable cv;
    bool terminate{ false };
};

} // namespace mbgl
