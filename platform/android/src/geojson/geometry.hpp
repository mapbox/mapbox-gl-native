#pragma once

#include <mbgl/util/geometry.hpp>

#include "../java/util.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class Geometry {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/Geometry"; };

    static jni::Local<jni::Object<Geometry>> New(jni::JNIEnv&, mbgl::Geometry<double>);

    static mbgl::Geometry<double> convert(jni::JNIEnv&, const jni::Object<Geometry>&);

    static std::string getType(jni::JNIEnv&, const jni::Object<Geometry>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl