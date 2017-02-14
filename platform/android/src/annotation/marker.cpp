#include "marker.hpp"

namespace mbgl {
namespace android {

jni::Class<Marker> Marker::javaClass;

mbgl::Point<double> Marker::getPosition(jni::JNIEnv& env, jni::Object<Marker> marker) {
    static auto positionField = Marker::javaClass.GetField<jni::Object<LatLng>>(env, "position");
    auto position = marker.Get(env, positionField);
    return LatLng::getGeometry(env, position);
}

std::string Marker::getIconId(jni::JNIEnv& env, jni::Object<Marker> marker) {
    static auto iconIdField = Marker::javaClass.GetField<jni::String>(env, "iconId");
    auto jIconId = marker.Get(env, iconIdField);
    return jni::Make<std::string>(env, jIconId);
}

void Marker::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Marker::javaClass = *jni::Class<Marker>::Find(env).NewGlobalRef(env).release();
}


} // namespace android
} // namespace mbgl