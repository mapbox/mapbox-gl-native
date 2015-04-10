#ifndef MBGL_UTIL_THREAD
#define MBGL_UTIL_THREAD

#include <future>
#include <thread>

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
    std::thread thread;
    std::promise<void> joinable;
    Object& object;
};

template <class Object>
template <class... Args>
Thread<Object>::Thread(Args&&... args)
    : object([&]() -> Object& {
          std::promise<Object&> promise;
          thread = std::thread([&] {
              Object context(::std::forward<Args>(args)...);
              promise.set_value(context);
              context.start();
              joinable.get_future().get();
          });
          return promise.get_future().get();
      }()) {
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
