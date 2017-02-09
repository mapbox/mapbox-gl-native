#include "interval_stops.hpp"

namespace mbgl {
namespace android {

jni::Object<Stop::CompositeValue> Stop::CompositeValue::New(jni::JNIEnv& env, jni::Object<java::lang::Number> zoom, jni::Object<> value) {
    static auto constructor = Stop::CompositeValue::javaClass.GetConstructor<jni::Object<java::lang::Number>, jni::Object<>>(env);
    return Stop::CompositeValue::javaClass.New(env, constructor, zoom, value);
}

jni::Class<Stop> Stop::javaClass;

jni::Class<Stop::CompositeValue> Stop::CompositeValue::javaClass;

void Stop::registerNative(jni::JNIEnv& env) {
    Stop::javaClass = *jni::Class<Stop>::Find(env).NewGlobalRef(env).release();
    Stop::CompositeValue::javaClass = *jni::Class<Stop::CompositeValue>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
