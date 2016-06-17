#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <stdexcept>

#include <pthread.h>

namespace mbgl {
namespace util {

template <class T>
class ThreadLocal : public noncopyable {
public:
    ThreadLocal(T* val) {
        ThreadLocal();
        set(val);
    }

    ThreadLocal() {
        int ret = pthread_key_create(&key, [](void *ptr) {
            delete reinterpret_cast<T *>(ptr);
        });

        if (ret) {
            throw std::runtime_error("Failed to init local storage key.");
        }
    }

    ~ThreadLocal() {
        if (pthread_key_delete(key)) {
            throw std::runtime_error("Failed to delete local storage key.");
        }
    }

    T* get() {
        T* ret = reinterpret_cast<T*>(pthread_getspecific(key));
        if (!ret) {
            return nullptr;
        }

        return ret;
    }

    void set(T* ptr) {
        if (pthread_setspecific(key, ptr)) {
            throw std::runtime_error("Failed to set local storage.");
        }
    }

private:
    pthread_key_t key;
};

} // namespace util
} // namespace mbgl
