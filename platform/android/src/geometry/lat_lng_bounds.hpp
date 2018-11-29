#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geometry.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class LatLngBounds : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/geometry/LatLngUnwrappedBounds"; };

    static jni::Local<jni::Object<LatLngBounds>> New(jni::JNIEnv&, mbgl::LatLngBounds);

    static mbgl::LatLngBounds getLatLngUnwrappedBounds(jni::JNIEnv &,
                                                       const jni::Object<LatLngBounds> &);

    static void registerNative(jni::JNIEnv&);
};


} // namespace android
} // namespace mbgl