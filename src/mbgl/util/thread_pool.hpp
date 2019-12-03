#pragma once

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <array>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace mbgl {

class ThreadedSchedulerBase : public Scheduler {
public:
    void schedule(std::function<void()>) override;

protected:
    ThreadedSchedulerBase() = default;
    ~ThreadedSchedulerBase() override;

    void terminate();
    std::thread makeSchedulerThread(size_t index);

    std::queue<std::function<void()>> queue;
    std::mutex mutex;
    std::condition_variable cv;
    bool terminated{false};
};

/**
 * @brief ThreadScheduler implements Scheduler interface using a lightweight event loop
 *
 * @tparam N number of threads
 *
 * Note: If N == 1 all scheduled tasks are guaranteed to execute consequently;
 * otherwise, some of the scheduled tasks might be executed in parallel.
 */
template <std::size_t N>
class ThreadedScheduler : public ThreadedSchedulerBase {
public:
    ThreadedScheduler() {
        for (std::size_t i = 0u; i < N; ++i) {
            threads[i] = makeSchedulerThread(i);
        }
    }

    ~ThreadedScheduler() override {
        terminate();
        for (auto& thread : threads) {
            thread.join();
        }
    }

    mapbox::base::WeakPtr<Scheduler> makeWeakPtr() override { return weakFactory.makeWeakPtr(); }

private:
    std::array<std::thread, N> threads;
    mapbox::base::WeakPtrFactory<Scheduler> weakFactory{this};
    static_assert(N > 0, "Thread count must be more than zero.");
};

class SequencedScheduler : public ThreadedScheduler<1> {};

template <std::size_t extra>
using ParallelScheduler = ThreadedScheduler<1 + extra>;

class ThreadPool : public ParallelScheduler<3> {};

} // namespace mbgl
