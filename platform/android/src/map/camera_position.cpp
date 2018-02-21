#include "camera_position.hpp"
#include "../geometry/lat_lng.hpp"

namespace mbgl {
namespace android {

jni::Object<CameraPosition> CameraPosition::New(jni::JNIEnv &env, mbgl::CameraOptions options) {
    static auto constructor = CameraPosition::javaClass.GetConstructor<jni::Object<LatLng>, double, double, double>(env);

    // wrap LatLng values coming from core
    auto center = options.center.value();
    center.wrap();

    // convert bearing, measured in radians counterclockwise from true north.
    // Wrapped to [−π rad, π rad). Android binding from 0 to 360 degrees
    double bearing_degrees = -options.angle.value_or(0) * util::RAD2DEG;
    while (bearing_degrees > 360) {
        bearing_degrees -= 360;
    }
    while (bearing_degrees < 0) {
        bearing_degrees += 360;
    }

    // convert tilt, core ranges from  [0 rad, 1,0472 rad], android ranges from 0 to 60
    double tilt_degrees = options.pitch.value_or(0) * util::RAD2DEG;

    return CameraPosition::javaClass.New(env, constructor, LatLng::New(env, center), options.zoom.value_or(0), tilt_degrees, bearing_degrees);
}

mbgl::CameraOptions CameraPosition::getCameraOptions(jni::JNIEnv& env, jni::Object<CameraPosition> position) {
    static auto bearing = CameraPosition::javaClass.GetField<jni::jdouble>(env, "bearing");
    static auto target = CameraPosition::javaClass.GetField<jni::Object<LatLng>>(env, "target");
    static auto tilt = CameraPosition::javaClass.GetField<jni::jdouble>(env, "tilt");
    static auto zoom = CameraPosition::javaClass.GetField<jni::jdouble>(env, "zoom");

    auto jtarget = position.Get(env, target);
    auto center = LatLng::getLatLng(env, jtarget);
    jni::DeleteLocalRef(env, jtarget);

    return mbgl::CameraOptions {
            center,
            {},
            {},
            position.Get(env, zoom),
            position.Get(env, bearing) * util::DEG2RAD,
            position.Get(env, tilt)
    };
}

void CameraPosition::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    CameraPosition::javaClass = *jni::Class<CameraPosition>::Find(env).NewGlobalRef(env).release();
}

jni::Class<CameraPosition> CameraPosition::javaClass;


} // namespace android
} // namespace mb

