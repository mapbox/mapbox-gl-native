#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include "geometry.hpp"
#include "../java/util.hpp"

#include <jni/jni.hpp>


namespace mbgl {
namespace android {
namespace geojson {

class Polygon : public Geometry {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/Polygon"; };

    static constexpr auto Type() { return "Polygon"; };

    static jni::Object<Polygon> New(jni::JNIEnv&, const mbgl::Polygon<double>&);

    static mapbox::geojson::polygon convert(jni::JNIEnv &, jni::Object<Polygon>);

    static mapbox::geojson::polygon convert(jni::JNIEnv&, jni::Object<java::util::List/*<java::util::List<Point>>*/>);

    static jni::Object<java::util::List> coordinates(jni::JNIEnv&, jni::Object<Polygon>);

    static jni::Class<Polygon> javaClass;

    static void registerNative(jni::JNIEnv &);
};

} // namespace geojson
} // namespace android
} // namespace mbgl