#pragma once

#include <future>
#include <thread>
#include <atomic>
#include <utility>
#include <functional>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread_context.hpp>
#include <mbgl/platform/platform.hpp>

#include <pthread.h>

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
    Thread(const ThreadContext&, Args... args);
    ~Thread();

    // Invoke object->fn(args...) in the runloop thread.
    template <typename Fn, class... Args>
    void invoke(Fn fn, Args... args) {
        loop->invoke(bind<Fn, void>(fn), std::move(args)...);
    }

    // Invoke object->fn(args...) in the runloop thread, then invoke callback(result) in the current thread.
    template <typename Fn, class Cb, class... Args>
    std::unique_ptr<AsyncRequest>
    invokeWithCallback(Fn fn, Cb callback, Args... args) {
        return loop->invokeWithCallback(bind<Fn, void>(fn), std::move(callback), std::move(args)...);
    }

    // Invoke object->fn(args...) in the runloop thread, and wait for the result.
    template <class R, typename Fn, class... Args>
    R invokeSync(Fn fn, Args... args) {
        std::packaged_task<R (Args...)> task(bind<Fn, R>(fn));
        std::future<R> future = task.get_future();
        loop->invoke(std::move(task), std::move(args)...);
        return future.get();
    }

    // Invoke object->fn(args...) in the runloop thread, and wait for it to complete.
    template <typename Fn, class... Args>
    void invokeSync(Fn fn, Args... args) {
        std::packaged_task<void (Args...)> task(bind<Fn, void>(fn));
        std::future<void> future = task.get_future();
        loop->invoke(std::move(task), std::move(args)...);
        future.get();
    }

private:
    Thread(const Thread&) = delete;
    Thread(Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
    Thread& operator=(Thread&&) = delete;

    template <class Fn, class R>
    auto bind(Fn fn) {
        return [fn, this] (auto... args) -> R {
             return (object->*fn)(std::move(args)...);
        };
    }

    template <typename P, std::size_t... I>
    void run(P&& params, std::index_sequence<I...>);

    std::promise<void> running;
    std::thread thread;

    Object* object = nullptr;
    RunLoop* loop = nullptr;
};

template <class Object>
template <class... Args>
Thread<Object>::Thread(const ThreadContext& context, Args... args) {
    // Note: We're using std::tuple<> to store the arguments because GCC 4.9 has a bug
    // when expanding parameters packs captured in lambdas.
    std::tuple<Args...> params = std::forward_as_tuple(std::move(args)...);

    thread = std::thread([&] {
        platform::setCurrentThreadName(context.name);

        if (context.priority == ThreadPriority::Low) {
            platform::makeThreadLowPriority();
        }

        run(std::move(params), std::index_sequence_for<Args...>{});
    });

    running.get_future().get();
}

template <class Object>
template <typename P, std::size_t... I>
void Thread<Object>::run(P&& params, std::index_sequence<I...>) {
    RunLoop loop_(RunLoop::Type::New);
    loop = &loop_;

    running.set_value();

    Object object_(std::get<I>(std::forward<P>(params))...);
    object = &object_;

    loop_.run();

}

template <class Object>
Thread<Object>::~Thread() {
    loop->stop();
    thread.join();
}

} // namespace util
} // namespace mbgl
