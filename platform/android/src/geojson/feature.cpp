#include "feature.hpp"

#include "geometry.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mbgl::Feature Feature::convert(jni::JNIEnv& env, jni::Object<Feature> jFeature) {
    // Convert
    auto jGeometry = getGeometry(env, jFeature);
    auto jProperties = Feature::getProperties(env, jFeature);

    std::experimental::optional<mapbox::geometry::identifier> id;
    auto jId = Feature::getId(env, jFeature);
    if (jId) {
        id = {  jni::Make<std::string>(env, jId) };
    }

    auto feature = mbgl::Feature {
            Geometry::convert(env, jGeometry),
            gson::JsonObject::convert(env, jProperties),
            id
    };

    // Cleanup
    jni::DeleteLocalRef(env, jGeometry);
    jni::DeleteLocalRef(env, jProperties);
    jni::DeleteLocalRef(env, jId);

    return feature;
}

jni::Object<Geometry> Feature::getGeometry(jni::JNIEnv& env, jni::Object<Feature> jFeature) {
    static auto method = Feature::javaClass.GetMethod<jni::Object<Geometry> ()>(env, "getGeometry");
    return jFeature.Call(env, method);
}

jni::Object<gson::JsonObject> Feature::getProperties(jni::JNIEnv& env, jni::Object<Feature> jFeature) {
    static auto method = Feature::javaClass.GetMethod<jni::Object<gson::JsonObject> ()>(env, "getProperties");
    return jFeature.Call(env, method);
}

jni::String Feature::getId(jni::JNIEnv& env, jni::Object<Feature> jFeature) {
    static auto method = Feature::javaClass.GetMethod<jni::String ()>(env, "getId");
    return jFeature.Call(env, method);
}

jni::Object<Feature> Feature::fromGeometry(jni::JNIEnv& env, jni::Object<Geometry> geometry, jni::Object<gson::JsonObject> properties, jni::String id) {
    static auto method = Feature::javaClass.GetStaticMethod<jni::Object<Feature> (jni::Object<Geometry>, jni::Object<gson::JsonObject>, jni::String)>(env, "fromGeometry");
    return Feature::javaClass.Call(env, method, geometry, properties, id);
}

void Feature::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Feature::javaClass = *jni::Class<Feature>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Feature> Feature::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl