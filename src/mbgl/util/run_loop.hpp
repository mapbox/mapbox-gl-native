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

    static RunLoop* Get() {
        return current.get();
    }

    static uv_loop_t* getLoop() {
        return current.get()->get();
    }

    void stop();

    // Invoke fn(args...) on this RunLoop.
    template <class Fn, class... Args>
    void invoke(Fn&& fn, Args&&... args) {
        auto tuple = std::make_tuple(std::move(args)...);
        auto task = std::make_shared<Invoker<Fn, decltype(tuple)>>(
            std::move(fn),
            std::move(tuple));

        withMutex([&] { queue.push(task); });
        async.send();
    }

    // Post the cancellable work fn(args...) to this RunLoop.
    template <class Fn, class... Args>
    std::unique_ptr<WorkRequest>
    invokeCancellable(Fn&& fn, Args&&... args) {
        auto flag = std::make_shared<bool>();
        *flag = false;

        auto tuple = std::make_tuple(std::move(args)...);
        auto task = std::make_shared<Invoker<Fn, decltype(tuple)>>(
            std::move(fn),
            std::move(tuple),
            flag);

        withMutex([&] { queue.push(task); });
        async.send();

        return std::make_unique<WorkRequest>(task);
    }

    // Invoke fn(args...) on this RunLoop, then invoke callback(results...) on the current RunLoop.
    template <class Fn, class Cb, class... Args>
    std::unique_ptr<WorkRequest>
    invokeWithCallback(Fn&& fn, Cb&& callback, Args&&... args) {
        auto flag = std::make_shared<bool>();
        *flag = false;

        auto after = RunLoop::current.get()->bind([flag, callback] (auto&&... results) {
            if (!*flag) {
                callback(std::move(results)...);
            }
        });

        auto tuple = std::make_tuple(std::move(args)..., after);
        auto task = std::make_shared<Invoker<Fn, decltype(tuple)>>(
            std::move(fn),
            std::move(tuple),
            flag);

        withMutex([&] { queue.push(task); });
        async.send();

        return std::make_unique<WorkRequest>(task);
    }

    // Return a function that invokes the given function on this RunLoop.
    template <class Fn>
    auto bind(Fn&& fn) {
        return [this, fn = std::move(fn)] (auto&&... args) {
            // `this->` is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61636
            this->invoke(std::move(fn), std::move(args)...);
        };
    }

    uv_loop_t* get() { return async.get()->loop; }

private:
    template <class F, class P>
    class Invoker : public WorkTask {
    public:
        Invoker(F&& f, P&& p, std::shared_ptr<bool> canceled_ = nullptr)
          : canceled(canceled_),
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
            func(std::get<I>(std::forward<P>(params))...);
        }

        std::recursive_mutex mutex;
        std::shared_ptr<bool> canceled;

        F func;
        P params;
    };

    using Queue = std::queue<std::shared_ptr<WorkTask>>;

    void withMutex(std::function<void()>&&);
    void process();

    Queue queue;
    std::recursive_mutex mutex;
    uv::async async;

    static uv::tls<RunLoop> current;
};

}
}

#endif
