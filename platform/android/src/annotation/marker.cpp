#include "marker.hpp"

namespace mbgl {
namespace android {

mbgl::Point<double> Marker::getPosition(jni::JNIEnv& env, jni::Object<Marker> marker) {
    static auto javaClass = jni::Class<Marker>::Singleton(env);
    static auto positionField = javaClass.GetField<jni::Object<LatLng>>(env, "position");
    return LatLng::getGeometry(env, *jni::SeizeLocal(env, marker.Get(env, positionField)));
}

std::string Marker::getIconId(jni::JNIEnv& env, jni::Object<Marker> marker) {
    static auto javaClass = jni::Class<Marker>::Singleton(env);
    static auto iconIdField = javaClass.GetField<jni::String>(env, "iconId");
    return jni::Make<std::string>(env, *jni::SeizeLocal(env, marker.Get(env, iconIdField)));
}

void Marker::registerNative(jni::JNIEnv& env) {
    jni::Class<Marker>::Singleton(env);
}

} // namespace android
} // namespace mbgl