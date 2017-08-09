#include <mbgl/util/thread_local.hpp>

#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/run_loop.hpp>

#include <stdexcept>
#include <cassert>

#include <pthread.h>

namespace mbgl {
namespace util {

template <class T>
class ThreadLocal<T>::Impl {
public:
    pthread_key_t key;
};

template <class T>
ThreadLocal<T>::ThreadLocal() : impl(std::make_unique<Impl>()) {
    int ret = pthread_key_create(&impl->key, [](void *) {});

    if (ret) {
        throw std::runtime_error("Failed to init local storage key.");
    }
}

template <class T>
ThreadLocal<T>::~ThreadLocal() {
    // ThreadLocal will not take ownership
    // of the pointer it is managing. The pointer
    // needs to be explicitly cleared before we
    // destroy this object.
    assert(!get());

    if (pthread_key_delete(impl->key)) {
        Log::Error(Event::General, "Failed to delete local storage key.");
        assert(false);
    }
}

template <class T>
T* ThreadLocal<T>::get() {
    auto* ret = reinterpret_cast<T*>(pthread_getspecific(impl->key));
    if (!ret) {
        return nullptr;
    }

    return ret;
}

template <class T>
void ThreadLocal<T>::set(T* ptr) {
    if (pthread_setspecific(impl->key, ptr)) {
        throw std::runtime_error("Failed to set local storage.");
    }
}

template class ThreadLocal<BackendScope>;
template class ThreadLocal<Scheduler>;
template class ThreadLocal<int>; // For unit tests

} // namespace util
} // namespace mbgl
