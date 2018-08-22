#pragma once

#include "geometry.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class GeometryCollection {
public:
    using SuperTag = Geometry;
    static constexpr auto Name() { return "com/mapbox/geojson/GeometryCollection"; };
    static constexpr auto Type() { return "GeometryCollection"; };

    static jni::Local<jni::Object<GeometryCollection>> New(jni::JNIEnv&, const mapbox::geometry::geometry_collection<double>&);

    static mapbox::geometry::geometry_collection<double> convert(jni::JNIEnv&, const jni::Object<GeometryCollection>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl