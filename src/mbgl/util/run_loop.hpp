#ifndef MBGL_UTIL_RUN_LOOP
#define MBGL_UTIL_RUN_LOOP

#include <mbgl/util/uv_detail.hpp>

#include <functional>
#include <queue>
#include <mutex>

namespace mbgl {
namespace util {

class RunLoop {
public:
    RunLoop();

    void run();
    void stop();

    // Invoke fn() in the runloop thread.
    template <class Fn>
    void invoke(Fn&& fn) {
        withMutex([&] { queue.push(Message(std::move(fn))); });
        async.send();
    }

    // Invoke fn() in the runloop thread, then invoke callback(result) in the current thread.
    template <class Fn, class R>
    void invokeWithResult(Fn&& fn, std::function<void (R)> callback) {
        RunLoop* outer = current.get();
        assert(outer);

        invoke([fn, callback, outer] {
            /*
                With C++14, we could write:

                outer->invoke([callback, result = std::move(fn())] () mutable {
                    callback(std::move(result));
                });

                Instead we're using a workaround with std::bind
                to obtain move-capturing semantics with C++11:
                  http://stackoverflow.com/a/12744730/52207
            */
            outer->invoke(std::bind([callback] (R& result) {
                callback(std::move(result));
            }, std::move(fn())));
        });
    }

    uv_loop_t* get() { return *loop; }

private:
    struct Message {
        struct Base {
            virtual void operator()() = 0;
            virtual ~Base() = default;
        };

        template <class F>
        struct Invoker : Base {
            Invoker(F&& f) : func(std::move(f)) {}
            void operator()() override { func(); }
            F func;
        }; 

        Message() = default;
        Message(Message&&) = default;
        ~Message() = default;
        Message& operator=(Message&&) = default;

        // copy members implicitly deleted

        template <class Fn>
        Message(Fn fn)
            : p_fn(new Invoker<Fn>(std::move(fn))) {
        }

        void operator()() const { (*p_fn)(); }
        std::unique_ptr<Base> p_fn;
    };

    using Queue = std::queue<Message>;

    static uv::tls<RunLoop> current;

    void withMutex(std::function<void()>&&);
    void process();

    Queue queue;
    std::mutex mutex;

    uv::loop loop;
    uv::async async;
};

}
}

#endif
