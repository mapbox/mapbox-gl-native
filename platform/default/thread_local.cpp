#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread_local.hpp>

#include <stdexcept>

#include <pthread.h>

namespace mbgl {
namespace util {

template class ThreadLocal<RunLoop>;
template class ThreadLocal<int>;
template class ThreadLocal<style::ClassDictionary>;

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
    delete reinterpret_cast<T *>(get());

    if (pthread_key_delete(impl->key)) {
        throw std::runtime_error("Failed to delete local storage key.");
    }
}

template <class T>
T* ThreadLocal<T>::get() {
    T* ret = reinterpret_cast<T*>(pthread_getspecific(impl->key));
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

} // namespace util
} // namespace mbgl
