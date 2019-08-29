#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/map/camera.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class CameraPosition : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/camera/CameraPosition"; };

    static jni::Local<jni::Object<CameraPosition>> New(jni::JNIEnv&, mbgl::CameraOptions, float pixelRatio);

    static mbgl::CameraOptions getCameraOptions(jni::JNIEnv&, const jni::Object<CameraPosition>&, float pixelRatio);

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl