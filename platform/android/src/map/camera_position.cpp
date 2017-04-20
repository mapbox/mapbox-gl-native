#include "camera_position.hpp"
#include "../geometry/lat_lng.hpp"

namespace mbgl {
namespace android {

jni::Object<CameraPosition> CameraPosition::New(jni::JNIEnv &env, mbgl::CameraOptions options) {
    static auto constructor = CameraPosition::javaClass.GetConstructor<jni::Object<LatLng>, double, double, double>(env);
    auto center = options.center.value();
    center.wrap();
    return CameraPosition::javaClass.New(env, constructor, LatLng::New(env, center), options.zoom.value_or(0), options.pitch.value_or(0), options.angle.value_or(0));
}

void CameraPosition::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    CameraPosition::javaClass = *jni::Class<CameraPosition>::Find(env).NewGlobalRef(env).release();
}

jni::Class<CameraPosition> CameraPosition::javaClass;


} // namespace android
} // namespace mb

