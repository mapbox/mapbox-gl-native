#pragma once

#include <type_traits>

#ifdef DETECT_THREAD_LOCAL
#include "thread_local_compiler_detection.h"
#endif

#ifdef __APPLE__
# include <TargetConditionals.h>
# include <Availability.h>
# if defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_9_0
#   undef MB_COMPILER_CXX_THREAD_LOCAL
# endif
# if defined(__i386__) && defined(TARGET_OS_SIMULATOR)
#   undef MB_COMPILER_CXX_THREAD_LOCAL
# endif
#endif

namespace mbgl {
namespace util {
namespace impl {

class ThreadLocalBase {
protected:
    ThreadLocalBase();
    ~ThreadLocalBase();

    void* get();
    void set(void*);

private:
#ifndef MB_COMPILER_CXX_THREAD_LOCAL
    std::aligned_storage_t<sizeof(void*), alignof(void*)> storage;
#endif
};

} // namespace impl

template <class T>
class ThreadLocal : public impl::ThreadLocalBase {
public:
    ThreadLocal() = default;

    ThreadLocal(T* val) {
        set(val);
    }

    T* get() {
        return reinterpret_cast<T*>(impl::ThreadLocalBase::get());
    }

    void set(T* ptr) {
        impl::ThreadLocalBase::set(ptr);
    }
};

} // namespace util
} // namespace mbgl
