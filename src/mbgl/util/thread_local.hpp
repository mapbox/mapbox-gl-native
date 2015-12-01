#ifndef MBGL_UTIL_THREAD_LOCAL
#define MBGL_UTIL_THREAD_LOCAL

#include <mbgl/util/noncopyable.hpp>

#include <stdexcept>

#include <pthread.h>

namespace mbgl {
namespace util {

template <class T>
class ThreadLocal : public noncopyable {
public:
    inline ThreadLocal(T* val) {
        ThreadLocal();
        set(val);
    }

    inline ThreadLocal() {
        int ret = pthread_key_create(&key, [](void *ptr) {
            delete reinterpret_cast<T *>(ptr);
        });

        if (ret) {
            throw new std::runtime_error("Failed to init local storage key.");
        }
    }

    inline ~ThreadLocal() {
        if (pthread_key_delete(key)) {
            throw new std::runtime_error("Failed to delete local storage key.");
        }
    }

    inline T* get() {
        T* ret = reinterpret_cast<T*>(pthread_getspecific(key));
        if (!ret) {
            return nullptr;
        }

        return ret;
    }

    inline void set(T* ptr) {
        if (pthread_setspecific(key, ptr)) {
            throw new std::runtime_error("Failed to set local storage.");
        }
    }

private:
    pthread_key_t key;
};

} // namespace util
} // namespace mbgl

#endif
