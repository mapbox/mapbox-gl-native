#ifndef MBGL_UTIL_THREAD
#define MBGL_UTIL_THREAD

#include <future>
#include <thread>

namespace mbgl {
namespace util {

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
          });
          return promise.get_future().get();
      }()) {
}

template <class Object>
Thread<Object>::~Thread() {
    object.stop();
    thread.join();
}

}
}

#endif
