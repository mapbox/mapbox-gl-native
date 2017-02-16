#pragma once

#include <jni/jni.hpp>

#include "../attach_env.hpp"

namespace mbgl {
namespace android {

// A deleter that doesn't retain an JNIEnv handle but instead tries to attach the JVM. This means
// it can be used on any thread to delete a global ref.
struct GenericGlobalRefDeleter {
    void operator()(jni::jobject* p) const {
        if (p) {
            auto env = AttachEnv();
            env->DeleteGlobalRef(jni::Unwrap(p));
        }
    }
};

} // namespace android
} // namespace mbgl
