#ifndef MBGL_UTIL_RUN_LOOP
#define MBGL_UTIL_RUN_LOOP

#include <mbgl/util/noncopyable.hpp>
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
        auto invokable = std::make_unique<Invoker<Fn, decltype(tuple), Args...>>(std::move(fn), std::move(tuple));
        withMutex([&] { queue.push(std::move(invokable)); });
        async.send();
    }

    // Return a function that invokes the given function on this RunLoop.
    template <class... Args>
    auto bind(std::function<void (Args...)> fn) {
        return [this, fn = std::move(fn)] (Args&&... args) {
            invoke(std::move(fn), std::move(args)...);
        };
    }

    // Invoke fn(args...) on this RunLoop, then invoke callback(result) on the current RunLoop.
    template <class R, class Fn, class... Args>
    void invokeWithResult(Fn&& fn, std::function<void (R)> callback, Args&&... args) {
        invoke([fn = std::move(fn), callback = current.get()->bind(callback)] (Args&&... a) mutable {
            callback(fn(std::forward<Args>(a)...));
        }, std::forward<Args>(args)...);
    }

    // Invoke fn(args...) on this RunLoop, then invoke callback() on the current RunLoop.
    template <class Fn, class... Args>
    void invokeWithResult(Fn&& fn, std::function<void ()> callback, Args&&... args) {
        invoke([fn = std::move(fn), callback = current.get()->bind(callback)] (Args&&... a) mutable {
            fn(std::forward<Args>(a)...);
            callback();
        }, std::forward<Args>(args)...);
    }

    uv_loop_t* get() { return async.get()->loop; }

    static uv::tls<RunLoop> current;

private:
    // A movable type-erasing invokable entity wrapper. This allows to store arbitrary invokable
    // things (like std::function<>, or the result of a movable-only std::bind()) in the queue.
    // Source: http://stackoverflow.com/a/29642072/331379
    struct Message {
        virtual void operator()() = 0;
        virtual ~Message() = default;
    };

    template <class F, class P, class... Args>
    struct Invoker : Message {
        Invoker(F&& f, P&& p)
          : func(std::move(f)),
            params(std::move(p)) {
        }

        void operator()() override {
            invoke(std::index_sequence_for<Args...>{});
        }

        template <std::size_t... I>
        void invoke(std::index_sequence<I...>) {
             func(std::forward<Args>(std::get<I>(params))...);
        }

        F func;
        P params;
    };

    using Queue = std::queue<std::unique_ptr<Message>>;

    void withMutex(std::function<void()>&&);
    void process();

    Queue queue;
    std::mutex mutex;
    uv::async async;
};

}
}

#endif
