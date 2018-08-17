#include "feature_collection.hpp"

#include "feature.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mbgl::FeatureCollection FeatureCollection::convert(jni::JNIEnv& env, jni::Object<FeatureCollection> jCollection) {
    auto collection = mbgl::FeatureCollection();

    if (jCollection) {
        auto jFeatureList = jni::SeizeLocal(env, FeatureCollection::features(env, jCollection));
        auto jFeatures = jni::SeizeLocal(env, java::util::List::toArray<Feature>(env, *jFeatureList));
        auto size = size_t(jFeatures->Length(env));
        collection.reserve(size);

        for (size_t i = 0; i < size; i++) {
            collection.push_back(Feature::convert(env, *jni::SeizeLocal(env, jFeatures->Get(env, i))));
        }
    }
    return collection;
}

jni::Object<java::util::List> FeatureCollection::features(jni::JNIEnv& env, jni::Object<FeatureCollection> jCollection) {
    static auto javaClass = jni::Class<FeatureCollection>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "features");
    return jCollection.Call(env, method);
}

void FeatureCollection::registerNative(jni::JNIEnv& env) {
    jni::Class<FeatureCollection>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl