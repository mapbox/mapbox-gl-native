#include "feature_collection.hpp"

#include "feature.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mbgl::FeatureCollection FeatureCollection::convert(jni::JNIEnv& env, jni::Object<FeatureCollection> jCollection) {
    auto jFeatureList = FeatureCollection::getFeatures(env, jCollection);
    auto jFeatures = java::util::List::toArray<Feature>(env, jFeatureList);
    auto size = size_t(jFeatures.Length(env));

    auto collection = mbgl::FeatureCollection();
    collection.reserve(size);

    for (size_t i = 0; i < size; i++) {
        auto jFeature = jFeatures.Get(env, i);
        collection.push_back(Feature::convert(env, jFeature));
        jni::DeleteLocalRef(env, jFeature);
    }

    return collection;
}

jni::Object<java::util::List> FeatureCollection::getFeatures(jni::JNIEnv& env, jni::Object<FeatureCollection> jCollection) {
    static auto method = FeatureCollection::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "getFeatures");
    return jCollection.Call(env, method);
}

void FeatureCollection::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    javaClass = *jni::Class<FeatureCollection>::Find(env).NewGlobalRef(env).release();
}

jni::Class<FeatureCollection> FeatureCollection::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl