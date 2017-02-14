#include "throwable.hpp"

namespace mbgl {
namespace android {
namespace java {
namespace lang {

jni::Class<Throwable> Throwable::Class(jni::JNIEnv& env) {
    static auto clazz = *jni::Class<Throwable>::Find(env).NewGlobalRef(env).release();
    return clazz;
}

jni::String
Throwable::GetMessage(jni::JNIEnv& env, jni::Object<Throwable> throwable) {
    using Signature = jni::String(void);
    static auto method = Class(env).GetMethod<Signature>(env, "getMessage");
    return throwable.Call(env, method);
}

} // namespace lang
} // namespace java
} // namespace android
} // namespace mbgl
