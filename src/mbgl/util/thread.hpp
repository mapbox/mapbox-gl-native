#pragma once

#include <future>
#include <thread>
#include <atomic>
#include <utility>
#include <functional>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread_context.hpp>
#include <mbgl/util/platform.hpp>

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

    // Invoke object->fn(args...) asynchronously.
    template <typename Fn, class... Args>
    void invoke(Fn fn, Args&&... args) {
        loop->invoke(bind(fn), std::forward<Args>(args)...);
    }

    // Invoke object->fn(args...) asynchronously. The final argument to fn must be a callback.
    // The provided callback is wrapped such that it is invoked, in the current thread (which
    // must have a RunLoop), once for each time the invocation of fn invokes the wrapper, each
    // time forwarding the passed arguments, until such time as the AsyncRequest is cancelled.
    template <typename Fn, class... Args>
    std::unique_ptr<AsyncRequest>
    invokeWithCallback(Fn fn, Args&&... args) {
        return loop->invokeWithCallback(bind(fn), std::forward<Args>(args)...);
    }

    // Invoke object->fn(args...) asynchronously, but wait for the result.
    template <typename Fn, class... Args>
    auto invokeSync(Fn fn, Args&&... args) {
        using R = std::result_of_t<Fn(Object, Args&&...)>;
        std::packaged_task<R ()> task(std::bind(fn, object, args...));
        std::future<R> future = task.get_future();
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
    void run(P&& params, std::index_sequence<I...>);

    std::promise<void> running;
    std::promise<void> joinable;

    std::thread thread;

    Object* object = nullptr;
    RunLoop* loop = nullptr;
};

template <class Object>
template <class... Args>
Thread<Object>::Thread(const ThreadContext& context, Args&&... args) {
    // Note: We're using std::tuple<> to store the arguments because GCC 4.9 has a bug
    // when expanding parameters packs captured in lambdas.
    std::tuple<Args...> params = std::forward_as_tuple(::std::forward<Args>(args)...);

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

    Object object_(std::get<I>(std::forward<P>(params))...);
    object = &object_;

    running.set_value();
    loop_.run();

    loop = nullptr;
    object = nullptr;

    joinable.get_future().get();
}

template <class Object>
Thread<Object>::~Thread() {
    loop->stop();
    joinable.set_value();
    thread.join();
}

} // namespace util
} // namespace mbgl
