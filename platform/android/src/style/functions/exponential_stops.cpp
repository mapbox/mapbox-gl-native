#include "exponential_stops.hpp"

namespace mbgl {
namespace android {

jni::Object<ExponentialStops> ExponentialStops::New(jni::JNIEnv& env, jni::Object<java::lang::Float> base, jni::Array<jni::Object<Stop>> stops) {
    static auto constructor = ExponentialStops::javaClass.GetConstructor<jni::Object<java::lang::Float>, jni::Array<jni::Object<Stop>>>(env);
    return ExponentialStops::javaClass.New(env, constructor, base, stops);
}

jni::Class<ExponentialStops> ExponentialStops::javaClass;

void ExponentialStops::registerNative(jni::JNIEnv& env) {
    ExponentialStops::javaClass = *jni::Class<ExponentialStops>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
