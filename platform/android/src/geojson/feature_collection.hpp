#pragma once

#include "../java/util.hpp"

#include <mbgl/util/geojson.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class FeatureCollection {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/FeatureCollection"; };

    static mbgl::FeatureCollection convert(jni::JNIEnv&, const jni::Object<FeatureCollection>&);

    static jni::Local<jni::Object<java::util::List>> features(jni::JNIEnv&, const jni::Object<FeatureCollection>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl