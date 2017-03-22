#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include "position.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class Point : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/services/commons/geojson/Point"; };

    static constexpr auto Type() { return "Point"; };

    static mapbox::geojson::point convert(jni::JNIEnv&, jni::Object<Point>);

    static jni::Object<Position> getPosition(JNIEnv&, jni::Object<Point>);

    static jni::Class<Point> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl