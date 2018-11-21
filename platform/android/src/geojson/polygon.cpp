#include "polygon.hpp"

#include "multi_line_string.hpp"

#include "util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Local<jni::Object<Polygon>> Polygon::New(jni::JNIEnv& env, const mbgl::Polygon<double>& polygon) {
    static auto& javaClass = jni::Class<Polygon>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<Polygon> (jni::Object<java::util::List>)>(env, "fromLngLats");

    return javaClass.Call(env, method, asPointsListsList(env, polygon));
}

mapbox::geojson::polygon Polygon::convert(jni::JNIEnv &env, const jni::Object<Polygon>& jPolygon) {
    mapbox::geojson::polygon polygon;

    if (jPolygon) {
        polygon = Polygon::convert(env, Polygon::coordinates(env, jPolygon));
    }

    return polygon;
}

mapbox::geojson::polygon Polygon::convert(jni::JNIEnv &env, const jni::Object<java::util::List/*<java::util::List<Point>>*/>& jPointListsList) {
    mapbox::geojson::polygon polygon;

    if (jPointListsList) {
        auto multiLine = MultiLineString::convert(env, jPointListsList);
        polygon.reserve(multiLine.size());
        for (auto&& line : multiLine) {
            polygon.emplace_back(convertExplicit<mapbox::geojson::linear_ring>(std::move(line)));
        }
    }

    return polygon;
}


jni::Local<jni::Object<java::util::List>> Polygon::coordinates(jni::JNIEnv &env, const jni::Object<Polygon>& jPolygon) {
    static auto& javaClass = jni::Class<Polygon>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
    return jPolygon.Call(env, method);
}

void Polygon::registerNative(jni::JNIEnv &env) {
    jni::Class<Polygon>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl