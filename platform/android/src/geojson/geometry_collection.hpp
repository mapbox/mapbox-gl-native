#pragma once

#include "geometry.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class GeometryCollection : public Geometry {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/GeometryCollection"; };

    static constexpr auto Type() { return "GeometryCollection"; };

    static jni::Object<GeometryCollection> New(jni::JNIEnv&, const mapbox::geometry::geometry_collection<double>&);

    static mapbox::geometry::geometry_collection<double> convert(jni::JNIEnv&, jni::Object<GeometryCollection>);

    static jni::Class<GeometryCollection> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl