#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/map/camera.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class CameraPosition : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/camera/CameraPosition"; };

    static jni::Object<CameraPosition> New(jni::JNIEnv&, mbgl::CameraOptions);

    static mbgl::CameraOptions getCameraOptions(jni::JNIEnv&, jni::Object<CameraPosition>);

    static jni::Class<CameraPosition> javaClass;

    static void registerNative(jni::JNIEnv&);

};


} // namespace android
} // namespace mbgl