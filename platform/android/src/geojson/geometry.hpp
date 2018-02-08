#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class Geometry : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/Geometry"; };

    static mapbox::geojson::geometry convert(jni::JNIEnv&, jni::Object<Geometry>);

    static std::string getType(jni::JNIEnv&, jni::Object<Geometry>);

    static jni::Class<Geometry> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl