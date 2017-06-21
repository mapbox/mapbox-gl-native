#include "lat_lng_bounds.hpp"

namespace mbgl {
namespace android {

jni::Object<LatLngBounds> LatLngBounds::New(jni::JNIEnv& env, mbgl::LatLngBounds bounds) {
    static auto constructor = LatLngBounds::javaClass.GetConstructor<double, double, double, double>(env);
    return LatLngBounds::javaClass.New(env, constructor, bounds.north(), bounds.east(), bounds.south(), bounds.west());
}

mbgl::LatLngBounds LatLngBounds::getLatLngBounds(jni::JNIEnv& env, jni::Object<LatLngBounds> bounds) {
    static auto swLat = LatLngBounds::javaClass.GetField<jni::jdouble>(env, "latitudeSouth");
    static auto swLon = LatLngBounds::javaClass.GetField<jni::jdouble>(env, "longitudeWest");
    static auto neLat = LatLngBounds::javaClass.GetField<jni::jdouble>(env, "latitudeNorth");
    static auto neLon = LatLngBounds::javaClass.GetField<jni::jdouble>(env, "longitudeEast");
    return mbgl::LatLngBounds::hull(
        { bounds.Get(env, swLat), bounds.Get(env, swLon) },
        { bounds.Get(env, neLat), bounds.Get(env, neLon) }
    );
}

void LatLngBounds::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    LatLngBounds::javaClass = *jni::Class<LatLngBounds>::Find(env).NewGlobalRef(env).release();
}

jni::Class<LatLngBounds> LatLngBounds::javaClass;


} // namespace android
} // namespace mbgl