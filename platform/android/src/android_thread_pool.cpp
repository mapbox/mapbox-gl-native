#include "android_thread_pool.hpp"
#include <mbgl/util/platform.hpp>

#include <cassert>

namespace mbgl {
namespace android {

void AndroidThreadLifecycle::onThreadCreated() const {
    attach_jni_thread(theJVM, &env, platform::getCurrentThreadName());
}

void AndroidThreadLifecycle::onThreadDestroyed() const {
    detach_jni_thread(theJVM, &env, true);
}

//AndroidThreadPool::AndroidThreadPool(size_t count) : mbgl::ThreadPool(count, std::make_unique<AndroidThreadLifecycle>()) {}

const auto onThreadCreated =
    [] {
        JNIEnv *env = nullptr;
        attach_jni_thread(theJVM, &env, platform::getCurrentThreadName());
        return env;
    };

const auto onThreadDestroyed =
    [] (void* data) {
        assert(data);
        JNIEnv* env = static_cast<JNIEnv*>(data);
        detach_jni_thread(theJVM, &env, true);
    };

AndroidThreadPool::AndroidThreadPool(size_t count) : mbgl::ThreadPool(count, {onThreadCreated, onThreadDestroyed}) {}

} // namespace android
} // namespace mbgl
