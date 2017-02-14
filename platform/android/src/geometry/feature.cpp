#include "feature.hpp"

namespace mbgl {
namespace android {

jni::Object<Feature> Feature::fromGeometry(jni::JNIEnv& env, jni::Object<Geometry> geometry, jni::Object<JsonObject> properties, jni::String id) {
    static auto method = Feature::javaClass.GetStaticMethod<jni::Object<Feature> (jni::Object<Geometry>, jni::Object<JsonObject>, jni::String)>(env, "fromGeometry");
    return Feature::javaClass.Call(env, method, geometry, properties, id);
}

void Feature::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Feature::javaClass = *jni::Class<Feature>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Feature> Feature::javaClass;


} // namespace android
} // namespace mbgl