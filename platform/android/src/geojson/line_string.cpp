#include "line_string.hpp"
#include "point.hpp"
#include "util.hpp"
#include "../java/util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Object<LineString> LineString::New(jni::JNIEnv& env, const mbgl::LineString<double>& lineString) {
    static auto javaClass = jni::Class<LineString>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<LineString>(jni::Object<java::util::List>)>(env, "fromLngLats");

    return javaClass.Call(env, method,
        *jni::SeizeLocal(env, asPointsList(env, lineString)));
}

mapbox::geojson::line_string LineString::convert(jni::JNIEnv &env, jni::Object<LineString> jLineString) {
    mapbox::geojson::line_string lineString;

    if (jLineString) {
        lineString = LineString::convert(env, *jni::SeizeLocal(env, LineString::coordinates(env, jLineString)));
    }

    return lineString;
}

mapbox::geojson::line_string LineString::convert(jni::JNIEnv &env, jni::Object<java::util::List/*<Point>*/> jPointList) {
    mapbox::geojson::line_string lineString;

    if (jPointList) {
        auto jPointArray = jni::SeizeLocal(env, java::util::List::toArray<Point>(env, jPointList));
        auto size = jPointArray->Length(env);
        lineString.reserve(size);

        for (std::size_t i = 0; i < size; i++) {
            lineString.push_back(Point::convert(env, *jni::SeizeLocal(env, jPointArray->Get(env, i))));
        }
    }

    return lineString;
}

jni::Object<java::util::List> LineString::coordinates(jni::JNIEnv &env, jni::Object<LineString> jLineString) {
    static auto javaClass = jni::Class<LineString>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
    return jLineString.Call(env, method);
}

void LineString::registerNative(jni::JNIEnv &env) {
    jni::Class<LineString>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl