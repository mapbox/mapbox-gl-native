#include "camera_position.hpp"
#include "../geometry/lat_lng.hpp"

namespace mbgl {
namespace android {

jni::Local<jni::Object<CameraPosition>> CameraPosition::New(jni::JNIEnv &env, mbgl::CameraOptions options, float pixelRatio) {
    static auto& javaClass = jni::Class<CameraPosition>::Singleton(env);
    static auto constructor = javaClass.GetConstructor<jni::Object<LatLng>, double, double, double, jni::Array<jni::jdouble>>(env);

    // wrap LatLng values coming from core
    auto center = options.center.value();
    center.wrap();

    // convert bearing, measured in radians counterclockwise from true north.
    // Wrapped to [−π rad, π rad). Android binding from 0 to 360 degrees
    double bearing_degrees = options.bearing.value_or(0);
    while (bearing_degrees > 360) {
        bearing_degrees -= 360;
    }
    while (bearing_degrees < 0) {
        bearing_degrees += 360;
    }

    // convert tilt, core ranges from  [0 rad, 1,0472 rad], android ranges from 0 to 60
    double tilt_degrees = options.pitch.value_or(0);

    std::vector<jdouble> paddingVect;
    auto insets = options.padding.value_or(EdgeInsets {0, 0, 0, 0});
    auto padding = jni::Array<jni::jdouble>::New(env, 4);
    paddingVect.push_back(insets.left() * pixelRatio);
    paddingVect.push_back(insets.top() * pixelRatio);
    paddingVect.push_back(insets.right() * pixelRatio);
    paddingVect.push_back(insets.bottom() * pixelRatio);
    padding.SetRegion<std::vector<jni::jdouble>>(env, 0, paddingVect);

    return javaClass.New(env, constructor, LatLng::New(env, center), options.zoom.value_or(0), tilt_degrees, bearing_degrees, padding);
}

mbgl::CameraOptions CameraPosition::getCameraOptions(jni::JNIEnv& env, const jni::Object<CameraPosition>& position, float pixelRatio) {
    static auto& javaClass = jni::Class<CameraPosition>::Singleton(env);
    static auto bearing = javaClass.GetField<jni::jdouble>(env, "bearing");
    static auto target = javaClass.GetField<jni::Object<LatLng>>(env, "target");
    static auto tilt = javaClass.GetField<jni::jdouble>(env, "tilt");
    static auto zoom = javaClass.GetField<jni::jdouble>(env, "zoom");
    static auto paddingField = javaClass.GetField<jni::Array<jni::jdouble>>(env, "padding");

    auto padding = position.Get(env, paddingField);
    auto center = LatLng::getLatLng(env, position.Get(env, target));

    return mbgl::CameraOptions{center,
                               padding && padding.Length(env) == 4 ? EdgeInsets{padding.Get(env, 1) * pixelRatio,
                                                                                padding.Get(env, 0) * pixelRatio,
                                                                                padding.Get(env, 3) * pixelRatio,
                                                                                padding.Get(env, 2) * pixelRatio}
                                                                   : (EdgeInsets){},
                               {},
                               position.Get(env, zoom),
                               position.Get(env, bearing),
                               position.Get(env, tilt)};
}

void CameraPosition::registerNative(jni::JNIEnv &env) {
    jni::Class<CameraPosition>::Singleton(env);
}

} // namespace android
} // namespace mb

