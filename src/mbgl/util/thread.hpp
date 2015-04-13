#ifndef MBGL_UTIL_THREAD
#define MBGL_UTIL_THREAD

#include <future>
#include <thread>

namespace {

template <::std::size_t...>
struct index_sequence {};

template <::std::size_t N, ::std::size_t... I>
struct integer_sequence : integer_sequence<N - 1, N - 1, I...> {};

template <::std::size_t... I>
struct integer_sequence<0, I...> {
    using type = index_sequence<I...>;
};

}

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
    Thread(Args&&... args);
    Thread(const Thread&) = delete;
    Thread(Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
    Thread& operator=(Thread&&) = delete;
    ~Thread();

    inline Object* operator->() const { return &object; }
    inline operator Object*() const { return &object; }

private:
    template <typename P, std::size_t... I>
    void run(std::promise<Object&>& promise, P&& params, index_sequence<I...>) {
        Object context(std::get<I>(std::forward<P>(params))...);
        promise.set_value(context);
        context.start();
        joinable.get_future().get();
    }

private:
    std::thread thread;
    std::promise<void> joinable;
    Object& object;
};

template <class Object>
template <class... Args>
Thread<Object>::Thread(Args&&... args)
    : object([&](std::tuple<Args...>&& params) -> Object& {
          // Note: We're using std::tuple<> to store the arguments because GCC 4.9 has a bug
          // when expanding parameters packs captured in lambdas.
          std::promise<Object&> promise;
          constexpr auto seq = typename integer_sequence<sizeof...(Args)>::type();
          thread = std::thread([&] {
              run(promise, std::move(params), seq);
          });
          return promise.get_future().get();
      }(std::move(std::forward_as_tuple(::std::forward<Args>(args)...)))) {
}

template <class Object>
Thread<Object>::~Thread() {
    object.stop();
    joinable.set_value();
    thread.join();
}

}
}

#endif
