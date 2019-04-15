#pragma once
#include <mbgl/util/default_thread_pool.hpp>
#include "jni.hpp"

namespace mbgl {
namespace android {

class AndroidThreadLifecycle : public mbgl::ThreadLifecycle {
public:
    void onThreadCreated() const override;
    void onThreadDestroyed() const override;

private:
    mutable JNIEnv *env = nullptr;
};

class AndroidThreadPool : public mbgl::ThreadPool {
public:
    explicit AndroidThreadPool(size_t count);
};

} // namespace android
} // namespace mbgl
