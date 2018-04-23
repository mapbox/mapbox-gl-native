#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include "../java/util.hpp"
#include "geometry.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class Point : public Geometry {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/Point"; };

    static constexpr auto Type() { return "Point"; };

    static jni::Object<Point> New(jni::JNIEnv&, const mbgl::Point<double>&);

    static mapbox::geojson::point convert(jni::JNIEnv&, jni::Object<Point>);

    static mapbox::geojson::point convert(jni::JNIEnv&, jni::Object<java::util::List/*<Double>*/>);

    static jni::Object<java::util::List> coordinates(JNIEnv&, jni::Object<Point>);

    static jni::Class<Point> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl