#include "feature.hpp"

#include "geometry.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mbgl::Feature Feature::convert(jni::JNIEnv& env, jni::Object<Feature> jFeature) {
    auto jGeometry = jni::SeizeLocal(env, geometry(env, jFeature));
    auto jProperties = jni::SeizeLocal(env, Feature::properties(env, jFeature));

    std::experimental::optional<mapbox::geometry::identifier> id;
    auto jId = jni::SeizeLocal(env, Feature::id(env, jFeature));
    if (jId) {
        id = {  jni::Make<std::string>(env, *jId) };
    }

    return mbgl::Feature {
        Geometry::convert(env, *jGeometry),
        gson::JsonObject::convert(env, *jProperties),
        id
    };
}

jni::Object<Geometry> Feature::geometry(jni::JNIEnv& env, jni::Object<Feature> jFeature) {
    static auto javaClass = jni::Class<Feature>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<Geometry> ()>(env, "geometry");
    return jFeature.Call(env, method);
}

jni::Object<gson::JsonObject> Feature::properties(jni::JNIEnv& env, jni::Object<Feature> jFeature) {
    static auto javaClass = jni::Class<Feature>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<gson::JsonObject> ()>(env, "properties");
    return jFeature.Call(env, method);
}

jni::String Feature::id(jni::JNIEnv& env, jni::Object<Feature> jFeature) {
    static auto javaClass = jni::Class<Feature>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::String ()>(env, "id");
    return jFeature.Call(env, method);
}

jni::Object<Feature> Feature::fromGeometry(jni::JNIEnv& env, jni::Object<Geometry> geometry, jni::Object<gson::JsonObject> properties, jni::String id) {
    static auto javaClass = jni::Class<Feature>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<Feature> (jni::Object<Geometry>, jni::Object<gson::JsonObject>, jni::String)>(env, "fromGeometry");
    return javaClass.Call(env, method, geometry, properties, id);
}

void Feature::registerNative(jni::JNIEnv& env) {
    jni::Class<Feature>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl