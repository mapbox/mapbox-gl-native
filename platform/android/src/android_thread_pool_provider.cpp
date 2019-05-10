#include <mbgl/util/thread_pool_provider.hpp>
#include <mbgl/util/platform.hpp>
#include <cassert>
#include "jni.hpp"

namespace mbgl {
std::shared_ptr<ThreadPool> getThreadPool() {
    ThreadLifecycle lifecycle = {
            [] {
                using namespace android;
                JNIEnv* env = nullptr;
                attach_jni_thread(theJVM, &env, platform::getCurrentThreadName());
                return env;
            },
            [](ThreadLifecycle::ThreadData threadData_) {
                using namespace android;
                assert(threadData_);
                auto* env = static_cast<JNIEnv*>(threadData_);
                detach_jni_thread(theJVM, &env, true);
            }
    };
    return std::make_shared<ThreadPool>(4, lifecycle);
}
} // namespace mbgl