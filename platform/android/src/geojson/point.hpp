#pragma once

#include <mbgl/util/geometry.hpp>

#include "geometry.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class Point {
public:
    using SuperTag = Geometry;
    static constexpr auto Name() { return "com/mapbox/geojson/Point"; };
    static constexpr auto Type() { return "Point"; };

    static jni::Local<jni::Object<Point>> New(jni::JNIEnv&, const mbgl::Point<double>&);
    static mbgl::Point<double> convert(jni::JNIEnv&, const jni::Object<Point>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl