#include "categorical_stops.hpp"

namespace mbgl {
namespace android {

jni::Object<CategoricalStops> CategoricalStops::New(jni::JNIEnv& env, jni::Array<jni::Object<Stop>> stops) {
    static auto constructor = CategoricalStops::javaClass.GetConstructor<jni::Array<jni::Object<Stop>>>(env);
    return CategoricalStops::javaClass.New(env, constructor, stops);
}

jni::Class<CategoricalStops> CategoricalStops::javaClass;

void CategoricalStops::registerNative(jni::JNIEnv& env) {
    CategoricalStops::javaClass = *jni::Class<CategoricalStops>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
