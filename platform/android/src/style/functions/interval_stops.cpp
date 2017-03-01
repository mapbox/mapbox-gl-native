#include "interval_stops.hpp"

namespace mbgl {
namespace android {

jni::Object<IntervalStops> IntervalStops::New(jni::JNIEnv& env, jni::Array<jni::Object<Stop>> stops) {
    static auto constructor = IntervalStops::javaClass.GetConstructor<jni::Array<jni::Object<Stop>>>(env);
    return IntervalStops::javaClass.New(env, constructor, stops);
}

jni::Class<IntervalStops> IntervalStops::javaClass;

void IntervalStops::registerNative(jni::JNIEnv& env) {
    IntervalStops::javaClass = *jni::Class<IntervalStops>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
