#include "android_thread_pool.hpp"
#include <mbgl/util/platform.hpp>

namespace mbgl {
namespace android {

void AndroidThreadLifecycle::onThreadCreated() const {
    attach_jni_thread(theJVM, &env, platform::getCurrentThreadName());
}

void AndroidThreadLifecycle::onThreadDestroyed() const {
    detach_jni_thread(theJVM, &env, true);
}

AndroidThreadPool::AndroidThreadPool(size_t count) : mbgl::ThreadPool(count, std::make_unique<AndroidThreadLifecycle>()) {}

} // namespace android
} // namespace mbgl
