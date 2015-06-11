#ifndef MBGL_UTIL_THREAD
#define MBGL_UTIL_THREAD

#include <future>
#include <thread>
#include <atomic>
#include <utility>
#include <functional>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread_context.hpp>
#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace util {

// Manages a thread with Object.

// Upon creation of this object, it launches a thread, creates an object of type Object in that
// thread, and then calls .start(); on that object. When the Thread<> object is destructed, the
// Object's .stop() function is called, and the destructor waits for thread termination. The
// Thread<> constructor blocks until the thread and the Object are fully created, so after the
// object creation, it's safe to obtain the Object stored in this thread.

template <class Object>
class Thread {
public:
    template <class... Args>
    Thread(const ThreadContext&, Args&&... args);
    ~Thread();

    // Invoke object->fn(args...) in the runloop thread.
    template <typename Fn, class... Args>
    void invoke(Fn fn, Args&&... args) {
        loop->invoke(bind(fn), std::forward<Args>(args)...);
    }

    // Invoke object->fn(args...) in the runloop thread, then invoke callback(result) in the current thread.
    template <typename Fn, class Cb, class... Args>
    std::unique_ptr<WorkRequest>
    invokeWithCallback(Fn fn, Cb&& callback, Args&&... args) {
        return loop->invokeWithCallback(bind(fn), callback, std::forward<Args>(args)...);
    }

    // Invoke object->fn(args...) in the runloop thread, and wait for the result.
    template <class R, typename Fn, class... Args>
    R invokeSync(Fn fn, Args&&... args) {
        std::packaged_task<R ()> task(std::bind(fn, object, args...));
        std::future<R> future = task.get_future();
        loop->invoke(std::move(task));
        return future.get();
    }

    // Invoke object->fn(args...) in the runloop thread, and wait for it to complete.
    template <typename Fn, class... Args>
    void invokeSync(Fn fn, Args&&... args) {
        std::packaged_task<void ()> task(std::bind(fn, object, args...));
        std::future<void> future = task.get_future();
        loop->invoke(std::move(task));
        return future.get();
    }

private:
    Thread(const Thread&) = delete;
    Thread(Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
    Thread& operator=(Thread&&) = delete;

    template <typename Fn>
    auto bind(Fn fn) {
        return [fn, this] (auto &&... args) {
            return (object->*fn)(std::forward<decltype(args)>(args)...);
        };
    }

    template <typename P, std::size_t... I>
    void run(ThreadContext, P&& params, std::index_sequence<I...>);

    std::promise<void> running;
    std::promise<void> joinable;

    std::thread thread;

    Object* object;
    RunLoop* loop;
};

template <class Object>
template <class... Args>
Thread<Object>::Thread(const ThreadContext& context, Args&&... args) {
    // Note: We're using std::tuple<> to store the arguments because GCC 4.9 has a bug
    // when expanding parameters packs captured in lambdas.
    std::tuple<Args...> params = std::forward_as_tuple(::std::forward<Args>(args)...);

    thread = std::thread([&] {
        #ifdef __APPLE__
        pthread_setname_np(context.name.c_str());
        #endif

        if (context.priority == ThreadPriority::Low) {
            platform::makeThreadLowPriority();
        }

        run(context, std::move(params), std::index_sequence_for<Args...>{});
    });

    running.get_future().get();
}

template <class Object>
template <typename P, std::size_t... I>
void Thread<Object>::run(ThreadContext context, P&& params, std::index_sequence<I...>) {
    uv::loop l;

    {
        ThreadContext::current.set(&context);

        RunLoop loop_(l.get());
        loop = &loop_;

        Object object_(l.get(), std::get<I>(std::forward<P>(params))...);
        object = &object_;

        running.set_value();
        l.run();

        loop = nullptr;
        object = nullptr;

        ThreadContext::current.set(nullptr);
    }

    // Run the loop again to ensure that async close callbacks have been called.
    l.run();

    joinable.get_future().get();
}

template <class Object>
Thread<Object>::~Thread() {
    loop->stop();
    joinable.set_value();
    thread.join();
}

}
}

#endif
