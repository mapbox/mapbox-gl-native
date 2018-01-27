#pragma once

#include <mbgl/util/feature.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

#include "geometry.hpp"
#include "../gson/json_object.hpp"

namespace mbgl {
namespace android {
namespace geojson {

class Feature : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/geojson/Feature"; };

    static jni::Object<Feature> fromGeometry(jni::JNIEnv&, jni::Object<Geometry>, jni::Object<gson::JsonObject>, jni::String);

    static mbgl::Feature convert(jni::JNIEnv&, jni::Object<Feature>);

    static jni::Object<Geometry> geometry(jni::JNIEnv&, jni::Object<Feature>);

    static jni::String id(jni::JNIEnv&, jni::Object<Feature>);

    static jni::Object<gson::JsonObject> properties(jni::JNIEnv&, jni::Object<Feature>);

    static jni::Class<Feature> javaClass;

    static void registerNative(jni::JNIEnv&);

};

} // namespace geojson
} // namespace android
} // namespace mbgl