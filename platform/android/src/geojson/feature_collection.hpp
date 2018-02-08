#pragma once

#include "../java/util.hpp"

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class FeatureCollection : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/FeatureCollection"; };

    static mbgl::FeatureCollection convert(jni::JNIEnv&, jni::Object<FeatureCollection>);

    static jni::Object<java::util::List> features(jni::JNIEnv&, jni::Object<FeatureCollection>);

    static jni::Class<FeatureCollection> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl