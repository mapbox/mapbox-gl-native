#include "line_string.hpp"

#include "point.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::line_string LineString::convert(jni::JNIEnv &env, jni::Object<LineString> jLineString) {
    mapbox::geojson::line_string lineString;

    if (jLineString) {
        auto jPointList = LineString::coordinates(env, jLineString);
        lineString = LineString::convert(env, jPointList);
        jni::DeleteLocalRef(env, jPointList);
    }

    return lineString;
}

mapbox::geojson::line_string LineString::convert(jni::JNIEnv &env, jni::Object<java::util::List/*<Point>*/> jPointList) {
    mapbox::geojson::line_string lineString;

    if (jPointList) {
        auto jPointArray = java::util::List::toArray<Point>(env, jPointList);
        auto size = jPointArray.Length(env);
        lineString.reserve(size);

        for (std::size_t i = 0; i < size; i++) {
            auto jPoint = jPointArray.Get(env, i);
            lineString.push_back(Point::convert(env, jPoint));
            jni::DeleteLocalRef(env, jPoint);
        }

        jni::DeleteLocalRef(env, jPointArray);
    }

    return lineString;
}

jni::Object<java::util::List> LineString::coordinates(jni::JNIEnv &env, jni::Object<LineString> jLineString) {
    static auto method = LineString::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
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