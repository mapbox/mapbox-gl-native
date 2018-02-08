#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

#include "../java/util.hpp"

namespace mbgl {
namespace android {
namespace geojson {


class LineString : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/LineString"; };

    static constexpr auto Type() { return "LineString"; };

    static mapbox::geojson::line_string convert(jni::JNIEnv&, jni::Object<LineString>);

    static mapbox::geojson::line_string convert(jni::JNIEnv&, jni::Object<java::util::List/*<Point>*/>);

    static jni::Object<java::util::List> coordinates(jni::JNIEnv&, jni::Object<LineString>);

    static jni::Class<LineString> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl