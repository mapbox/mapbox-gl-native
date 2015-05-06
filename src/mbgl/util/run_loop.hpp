#ifndef MBGL_UTIL_RUN_LOOP
#define MBGL_UTIL_RUN_LOOP

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <functional>
#include <queue>
#include <mutex>

namespace mbgl {
namespace util {

class RunLoop : private util::noncopyable {
public:
    RunLoop(uv_loop_t*);
    ~RunLoop();

    void stop();

    // Invoke fn() in the runloop thread.
    template <class Fn>
    void invoke(Fn&& fn) {
        auto invokable = util::make_unique<Invoker<Fn>>(std::move(fn));
        withMutex([&] { queue.push(std::move(invokable)); });
        async.send();
    }

    // Invoke fn() in the runloop thread, then invoke callback(result) in the current thread.
    template <class Fn, class R>
    void invokeWithResult(Fn&& fn, std::function<void (R)>&& callback) {
        RunLoop* outer = current.get();
        assert(outer);

        invoke([fn = std::move(fn), callback = std::move(callback), outer] () mutable {
            outer->invoke([callback = std::move(callback), result = std::move(fn())] () mutable {
                callback(std::move(result));
            });
        });
    }

    // Invoke fn() in the runloop thread, then invoke callback() in the current thread.
    template <class Fn>
    void invokeWithResult(Fn&& fn, std::function<void ()>&& callback) {
        RunLoop* outer = current.get();
        assert(outer);

        invoke([fn = std::move(fn), callback = std::move(callback), outer] () mutable {
            fn();
            outer->invoke(std::move(callback));
        });
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

    template <class F>
    struct Invoker : Message {
        Invoker(F&& f) : func(std::move(f)) {}
        void operator()() override { func(); }
        F func;
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
