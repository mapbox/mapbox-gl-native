#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class Position : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/services/commons/models/Position"; };

    static constexpr auto Type() { return "Position"; };

    static mapbox::geojson::point convert(jni::JNIEnv&, jni::Object<Position>);

    static jni::Class<Position> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl