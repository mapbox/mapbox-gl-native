#include "identity_stops.hpp"

namespace mbgl {
namespace android {

jni::Object<IdentityStops> IdentityStops::New(jni::JNIEnv& env) {
    static auto constructor = IdentityStops::javaClass.GetConstructor<>(env);
    return IdentityStops::javaClass.New(env, constructor);
}

jni::Class<IdentityStops> IdentityStops::javaClass;

void IdentityStops::registerNative(jni::JNIEnv& env) {
    IdentityStops::javaClass = *jni::Class<IdentityStops>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
