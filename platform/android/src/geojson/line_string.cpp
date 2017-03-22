#include "line_string.hpp"

#include "position.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::line_string LineString::convert(jni::JNIEnv &env, jni::Object<LineString> jLineString) {
    mapbox::geojson::line_string lineString;

    if (jLineString) {
        auto jPositionList = LineString::getCoordinates(env, jLineString);
        lineString = LineString::convert(env, jPositionList);
        jni::DeleteLocalRef(env, jPositionList);
    }

    return lineString;
}

mapbox::geojson::line_string LineString::convert(jni::JNIEnv &env, jni::Object<java::util::List/*<Position>*/> jPositionList) {
    mapbox::geojson::line_string lineString;

    if (jPositionList) {
        auto jPositionArray = java::util::List::toArray<Position>(env, jPositionList);

        auto size = jPositionArray.Length(env);
        for (std::size_t i = 0; i < size; i++) {
            auto jPosition = jPositionArray.Get(env, i);
            lineString.push_back(Position::convert(env, jPosition));
            jni::DeleteLocalRef(env, jPosition);
        }

        jni::DeleteLocalRef(env, jPositionArray);
    }

    return lineString;
}

jni::Object<java::util::List> LineString::getCoordinates(jni::JNIEnv &env, jni::Object<LineString> jLineString) {
    static auto method = LineString::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "getCoordinates");
    return jLineString.Call(env, method);
}

void LineString::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<LineString>::Find(env).NewGlobalRef(env).release();
}

jni::Class<LineString> LineString::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl