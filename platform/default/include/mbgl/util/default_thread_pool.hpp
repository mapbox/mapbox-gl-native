#pragma once

#include <mbgl/actor/scheduler.hpp>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace mbgl {

struct ThreadLifecycle {
    virtual void onThreadCreated() const = 0;
    virtual void onThreadDestroyed() const = 0;
    virtual ~ThreadLifecycle() = default;
};

struct ThreadLifecycleV2 {
    ThreadLifecycleV2() = default;
    ThreadLifecycleV2(std::function<void*()> onThreadCreated_,
                      std::function<void(void*)> onThreadDestroyed_) :
                      onThreadCreatedFn(onThreadCreated_),
                      onThreadDestroyedFn(onThreadDestroyed_) {}

    void onThreadCreated() const {
        privateData = onThreadCreatedFn();
    }

    void onThreadDestroyed() const {
        onThreadDestroyedFn(privateData);
    }

private:
    std::function<void*()> onThreadCreatedFn = []{return nullptr;};
    std::function<void(void*)> onThreadDestroyedFn = [](void*){};
    mutable void* privateData = NULL;
};

class ThreadPool : public Scheduler {
public:
    explicit ThreadPool(std::size_t count);
    ThreadPool(std::size_t count, std::unique_ptr<ThreadLifecycle> _lifecycle);
    ThreadPool(std::size_t count, ThreadLifecycleV2 = ThreadLifecycleV2());
    ~ThreadPool() override;

    void schedule(std::weak_ptr<Mailbox>) override;

private:
    std::unique_ptr<ThreadLifecycle> lifecycle;
    std::vector<std::thread> threads;
    std::queue<std::weak_ptr<Mailbox>> queue;
    std::mutex mutex;
    std::condition_variable cv;
    bool terminate { false };
};

} // namespace mbgl
