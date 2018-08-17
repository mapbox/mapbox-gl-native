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
            AttachEnv()->DeleteGlobalRef(jni::Unwrap(p));
        }
    }
};

struct GenericWeakGlobalRefDeleter {
    void operator()(jni::jobject* p) const {
        if (p) {
            AttachEnv()->DeleteWeakGlobalRef(jni::Unwrap(p));
        }
    }
};

template < class T >
using GenericGlobal = jni::UniquePointerlike< T, GenericGlobalRefDeleter >;

template < class T >
using GenericWeak = jni::UniquePointerlike< T, GenericWeakGlobalRefDeleter >;

template < class T >
GenericGlobal<T> SeizeGenericGlobal(T&& t) {
    return GenericGlobal<T>(std::move(t), GenericGlobalRefDeleter());
};

template < class T >
GenericWeak<T> SeizeGenericWeak(T&& t) {
    return GenericWeak<T>(std::move(t), GenericWeakGlobalRefDeleter());
};

} // namespace android
} // namespace mbgl
