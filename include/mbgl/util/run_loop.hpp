#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>

#include <atomic>
#include <functional>
#include <utility>
#include <queue>
#include <mutex>

namespace mbgl {
namespace util {

typedef void * LOOP_HANDLE;

class RunLoop : private util::noncopyable {
public:
    enum class Type : uint8_t {
        Default,
        New,
    };

    enum class Event : uint8_t {
        None      = 0,
        Read      = 1,
        Write     = 2,
        ReadWrite = Read | Write,
    };

    RunLoop(Type type = Type::Default);
    ~RunLoop();

    static RunLoop* Get();
    static LOOP_HANDLE getLoopHandle();

    void run();
    void runOnce();
    void stop();

    // So far only needed by the libcurl backend.
    void addWatch(int fd, Event, std::function<void(int, Event)>&& callback);
    void removeWatch(int fd);

    // Invoke fn(args...) on this RunLoop.
    template <class Fn, class... Args>
    void invoke(Fn&& fn, Args&&... args) {
        auto tuple = std::make_tuple(std::move(args)...);
        auto task = std::make_shared<Invoker<Fn, decltype(tuple)>>(
            std::move(fn),
            std::move(tuple));

        push(task);
    }

    // Post the cancellable work fn(args...) to this RunLoop.
    template <class Fn, class... Args>
    std::unique_ptr<AsyncRequest>
    invokeCancellable(Fn&& fn, Args&&... args) {
        auto flag = std::make_shared<std::atomic<bool>>();
        *flag = false;

        auto tuple = std::make_tuple(std::move(args)...);
        auto task = std::make_shared<Invoker<Fn, decltype(tuple)>>(
            std::move(fn),
            std::move(tuple),
            flag);

        push(task);

        return std::make_unique<WorkRequest>(task);
    }

    // Invoke fn(args...) on this RunLoop, then invoke callback(results...) on the current RunLoop.
    template <class Fn, class Cb, class... Args>
    std::unique_ptr<AsyncRequest>
    invokeWithCallback(Fn&& fn, Cb&& callback, Args&&... args) {
        auto flag = std::make_shared<std::atomic<bool>>();
        *flag = false;

        // Create a lambda L1 that invokes another lambda L2 on the current RunLoop R, that calls
        // the callback C. Both lambdas check the flag before proceeding. L1 needs to check the flag
        // because if the request was cancelled, then R might have been destroyed. L2 needs to check
        // the flag because the request may have been cancelled after L2 was invoked but before it
        // began executing.
        auto after = [flag, current = RunLoop::Get(), callback1 = std::move(callback)] (auto&&... results1) {
            if (!*flag) {
                current->invoke([flag, callback2 = std::move(callback1)] (auto&&... results2) {
                    if (!*flag) {
                        callback2(std::move(results2)...);
                    }
                }, std::move(results1)...);
            }
        };

        auto tuple = std::make_tuple(std::move(args)..., after);
        auto task = std::make_shared<Invoker<Fn, decltype(tuple)>>(
            std::move(fn),
            std::move(tuple),
            flag);

        push(task);

        return std::make_unique<WorkRequest>(task);
    }

    class Impl;

private:
    MBGL_STORE_THREAD(tid)

    template <class F, class P>
    class Invoker : public WorkTask {
    public:
        Invoker(F&& f, P&& p, std::shared_ptr<std::atomic<bool>> canceled_ = nullptr)
          : canceled(std::move(canceled_)),
            func(std::move(f)),
            params(std::move(p)) {
        }

        void operator()() override {
            // Lock the mutex while processing so that cancel() will block.
            std::lock_guard<std::recursive_mutex> lock(mutex);
            if (!canceled || !*canceled) {
                invoke(std::make_index_sequence<std::tuple_size<P>::value>{});
            }
        }

        // If the task has not yet begun, this will cancel it.
        // If the task is in progress, this will block until it completed. (Currently
        // necessary because of shared state, but should be removed.) It will also
        // cancel the after callback.
        // If the task has completed, but the after callback has not executed, this
        // will cancel the after callback.
        // If the task has completed and the after callback has executed, this will
        // do nothing.
        void cancel() override {
            std::lock_guard<std::recursive_mutex> lock(mutex);
            *canceled = true;
        }

    private:
        template <std::size_t... I>
        void invoke(std::index_sequence<I...>) {
            func(std::move(std::get<I>(std::forward<P>(params)))...);
        }

        std::recursive_mutex mutex;
        std::shared_ptr<std::atomic<bool>> canceled;

        F func;
        P params;
    };

    using Queue = std::queue<std::shared_ptr<WorkTask>>;

    void push(std::shared_ptr<WorkTask>);

    void withMutex(std::function<void()>&& fn) {
        std::lock_guard<std::mutex> lock(mutex);
        fn();
    }

    void process() {
        Queue queue_;
        withMutex([&] { queue_.swap(queue); });

        while (!queue_.empty()) {
            (*(queue_.front()))();
            queue_.pop();
        }
    }

    Queue queue;
    std::mutex mutex;

    std::unique_ptr<Impl> impl;
};

} // namespace util
} // namespace mbgl
