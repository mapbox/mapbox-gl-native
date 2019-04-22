#include <mbgl/util/thread_local.hpp>
#include <mbgl/util/logging.hpp>

#include <cassert>
#include <cstdlib>

#include <pthread.h>

namespace mbgl {
namespace util {
namespace impl {

ThreadLocalBase::ThreadLocalBase() {
    static_assert(sizeof(storage) >= sizeof(pthread_key_t), "storage is too small");
    static_assert(alignof(decltype(storage)) % alignof(pthread_key_t) == 0, "storage is incorrectly aligned");
    if (pthread_key_create(&reinterpret_cast<pthread_key_t&>(storage), nullptr) != 0) {
        Log::Error(Event::General, "Failed to initialize thread-specific storage key");
        abort();
    }
}

ThreadLocalBase::~ThreadLocalBase() {
    // ThreadLocal will not take ownership of the pointer it is managing. The pointer
    // needs to be explicitly cleared before we destroy this object.
    assert(!get());

    if (pthread_key_delete(reinterpret_cast<pthread_key_t&>(storage)) != 0) {
        Log::Error(Event::General, "Failed to delete thread-specific storage key");
        abort();
    }
}

void* ThreadLocalBase::get() {
    return pthread_getspecific(reinterpret_cast<pthread_key_t&>(storage));
}

void ThreadLocalBase::set(void* ptr) {
    if (pthread_setspecific(reinterpret_cast<pthread_key_t&>(storage), ptr) != 0) {
        Log::Error(Event::General, "Failed to set thread-specific storage");
        abort();
    }
}

} // namespace impl
} // namespace util
} // namespace mbgl
