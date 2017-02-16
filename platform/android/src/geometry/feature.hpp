#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geometry.hpp>

#include <jni/jni.hpp>

#include "geometry.hpp"
#include "../gson/json_object.hpp"

namespace mbgl {
namespace android {

class Feature : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/services/commons/geojson/Feature"; };

    static jni::Object<Feature> fromGeometry(jni::JNIEnv&, jni::Object<Geometry>, jni::Object<JsonObject>, jni::String);

    static jni::Class<Feature> javaClass;

    static void registerNative(jni::JNIEnv&);

};


} // namespace android
} // namespace mbgl