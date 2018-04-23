#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/noncopyable.hpp>

#include "../java/util.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class Geometry : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/Geometry"; };

    static jni::Object<Geometry> New(jni::JNIEnv&, mbgl::Geometry<double>);

    static mbgl::Geometry<double> convert(jni::JNIEnv&, jni::Object<Geometry>);

    static std::string getType(jni::JNIEnv&, jni::Object<Geometry>);

    static jni::Class<Geometry> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl