#ifndef MBGL_UTIL_RUN_LOOP
#define MBGL_UTIL_RUN_LOOP

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <functional>
#include <utility>
#include <queue>
#include <mutex>

namespace mbgl {
namespace util {

class RunLoop : private util::noncopyable {
public:
    RunLoop(uv_loop_t*);
    ~RunLoop();

    void stop();

    // Invoke fn(args...) on this RunLoop.
    template <class Fn, class... Args>
    void invoke(Fn&& fn, Args&&... args) {
        auto tuple = std::make_tuple(std::move(args)...);
        auto task = std::make_shared<Invoker<Fn, decltype(tuple), Args...>>(
            std::move(fn),
            std::move(tuple));

        withMutex([&] { queue.push(task); });
        async.send();
    }

    // Invoke fn(args...) on this RunLoop, then invoke callback() on the current RunLoop.
    template <class Fn, class... Args>
    std::unique_ptr<WorkRequest>
    invokeWithResult(Fn&& fn, std::function<void ()> callback, Args&&... args) {
        auto tuple = std::make_tuple(std::move(args)...);
        auto task = std::make_shared<Invoker<Fn, decltype(tuple), Args...>>(
            std::move(fn),
            std::move(tuple));

        // `task` is a shared pointer with ownership in the following three places:
        //   1. In the `queue` of pending invocations.
        //   2. In the `WorkRequest` result.
        //   3. In the lambda binding of the callback to be executed on the invoking
        //      RunLoop. This last shared ownership is necessary in the case where
        //      callback execution has been scheduled (queued on the invoking RunLoop),
        //      but the other two places have released ownership -- i.e. the task was
        //      cancelled after the work is completed, but before the callback is
        //      executed. In this case, the lambda binding checks the cancellation flag
        //      and does not execute the original callback.

        task->bind(callback);

        withMutex([&] { queue.push(task); });
        async.send();

        return std::make_unique<WorkRequest>(task);
    }

    // Invoke fn(args...) on this RunLoop, then invoke callback(result) on the current RunLoop.
    template <class R, class Fn, class... Args>
    std::unique_ptr<WorkRequest>
    invokeWithResult(Fn&& fn, std::function<void (R)> callback, Args&&... args) {
        auto tuple = std::make_tuple(std::move(args)...);
        auto task = std::make_shared<InvokerWithResult<Fn, decltype(tuple), R, Args...>>(
            std::move(fn),
            std::move(tuple));

        task->bind(callback);

        withMutex([&] { queue.push(task); });
        async.send();

        return std::make_unique<WorkRequest>(task);
    }

    // Return a function that invokes the given function on this RunLoop.
    template <class... Args>
    auto bind(std::function<void (Args...)> fn) {
        return [this, fn = std::move(fn)] (Args&&... args) {
            invoke(std::move(fn), std::move(args)...);
        };
    }

    uv_loop_t* get() { return async.get()->loop; }

    static uv::tls<RunLoop> current;

private:
    template <class F, class P, class... Args>
    class Invoker : public WorkTask, public std::enable_shared_from_this<Invoker<F, P, Args...>> {
    public:
        Invoker(F&& f, P&& p)
          : func(std::move(f)),
            params(std::move(p)) {
        }

        using C = std::function<void ()>;

        void bind(C after) {
            auto task = this->shared_from_this();
            callback = RunLoop::current.get()->bind(C([task, after] {
                if (!task->canceled) {
                    after();
                }
            }));
        }

        void operator()() override {
            // We are only running the task when there's an after callback to call. This means that an
            // empty after callback will be treated as a cancelled request. The mutex will be locked while
            // processing so that the cancel() callback will block.
            std::lock_guard<std::mutex> lock(mutex);
            if (!canceled) {
                invoke(std::index_sequence_for<Args...>{});
            }
        }

        void cancel() override {
            // Remove the after callback to indicate that this callback has been canceled. The mutex will
            // block when the task is currently in progres. When the task has not begun yet, the runTask()
            // method will not do anything. When the task has been completed already, and the after callback
            // was run as well, this will also do nothing.
            std::lock_guard<std::mutex> lock(mutex);
            canceled = true;
        }

    private:
        template <std::size_t... I>
        void invoke(std::index_sequence<I...>) {
            func(std::forward<Args>(std::get<I>(params))...);
            if (callback) {
                callback();
            }
        }

        std::mutex mutex;
        bool canceled = false;

        F func;
        P params;
        C callback;
    };

    template <class F, class P, class R, class... Args>
    class InvokerWithResult : public WorkTask, public std::enable_shared_from_this<InvokerWithResult<F, P, R, Args...>> {
    public:
        InvokerWithResult(F&& f, P&& p)
          : func(std::move(f)),
            params(std::move(p)) {
        }

        using C = std::function<void (R)>;

        void bind(C after) {
            auto task = this->shared_from_this();
            callback = RunLoop::current.get()->bind(C([task, after] (R result) {
                if (!task->canceled) {
                    after(std::move(result));
                }
            }));
        }

        void operator()() override {
            // We are only running the task when there's an after callback to call. This means that an
            // empty after callback will be treated as a cancelled request. The mutex will be locked while
            // processing so that the cancel() callback will block.
            std::lock_guard<std::mutex> lock(mutex);
            if (!canceled) {
                invoke(std::index_sequence_for<Args...>{});
            }
        }

        void cancel() override {
            // Remove the after callback to indicate that this callback has been canceled. The mutex will
            // block when the task is currently in progres. When the task has not begun yet, the runTask()
            // method will not do anything. When the task has been completed already, and the after callback
            // was run as well, this will also do nothing.
            std::lock_guard<std::mutex> lock(mutex);
            canceled = true;
        }

    private:
        template <std::size_t... I>
        void invoke(std::index_sequence<I...>) {
            callback(func(std::forward<Args>(std::get<I>(params))...));
        }

        std::mutex mutex;
        bool canceled = false;

        F func;
        P params;
        C callback;
    };

    using Queue = std::queue<std::shared_ptr<WorkTask>>;

    void withMutex(std::function<void()>&&);
    void process();

    Queue queue;
    std::mutex mutex;
    uv::async async;
};

}
}

#endif
