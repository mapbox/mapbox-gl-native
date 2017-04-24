#include "camera_position.hpp"
#include "../geometry/lat_lng.hpp"

namespace mbgl {
namespace android {

jni::Object<CameraPosition> CameraPosition::New(jni::JNIEnv &env, mbgl::CameraOptions options) {
    static auto constructor = CameraPosition::javaClass.GetConstructor<jni::Object<LatLng>, double, double, double>(env);

    // wrap LatLng values coming from core
    auto center = options.center.value();
    center.wrap();

    // convert bearing, core ranges from [−π rad, π rad], android from 0 to 360 degrees
    double bearing_degrees = options.angle.value_or(0) * 180.0 / M_PI;
    while (bearing_degrees > 360) {
        bearing_degrees -= 360;
    }
    while (bearing_degrees < 0) {
        bearing_degrees += 360;
    }

    // convert tilt, core ranges from  [0 rad, 1,0472 rad], android ranges from 0 to 60
    double tilt_degrees = options.pitch.value_or(0) * 180 / M_PI;

    return CameraPosition::javaClass.New(env, constructor, LatLng::New(env, center), options.zoom.value_or(0), tilt_degrees, bearing_degrees);
}

void CameraPosition::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    CameraPosition::javaClass = *jni::Class<CameraPosition>::Find(env).NewGlobalRef(env).release();
}

jni::Class<CameraPosition> CameraPosition::javaClass;


} // namespace android
} // namespace mb

