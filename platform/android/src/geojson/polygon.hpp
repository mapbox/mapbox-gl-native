#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

#include "../java/util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

class Polygon : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/services/commons/geojson/Polygon"; };

    static constexpr auto Type() { return "Polygon"; };

    static mapbox::geojson::polygon convert(jni::JNIEnv &, jni::Object<Polygon>);

    static mapbox::geojson::polygon convert(jni::JNIEnv&, jni::Object<java::util::List/*<java::util::List<Position>>*/>);

    static jni::Object<java::util::List> getCoordinates(jni::JNIEnv&, jni::Object<Polygon>);

    static jni::Class<Polygon> javaClass;

    static void registerNative(jni::JNIEnv &);
};

} // namespace geojson
} // namespace android
} // namespace mbgl