#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geometry.hpp>

#include <jni/jni.hpp>
#include <array>

namespace mbgl {
namespace android {

class LatLngQuad : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/geometry/LatLngQuad"; };

    static jni::Object<LatLngQuad> New(jni::JNIEnv&, std::array<mbgl::LatLng, 4>);

    static std::array<mbgl::LatLng, 4> getLatLngArray(jni::JNIEnv&, jni::Object<LatLngQuad>);

    static jni::Class<LatLngQuad> javaClass;

    static void registerNative(jni::JNIEnv&);

};


} // namespace android
} // namespace mbgl