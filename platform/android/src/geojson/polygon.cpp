#include "polygon.hpp"

#include "multi_line_string.hpp"

#include "util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::polygon Polygon::convert(jni::JNIEnv &env, jni::Object<Polygon> jPolygon) {
    mapbox::geojson::polygon polygon;

    if (jPolygon) {
        auto jPositionListsList = Polygon::coordinates(env, jPolygon);
        polygon = Polygon::convert(env, jPositionListsList);
        jni::DeleteLocalRef(env, jPositionListsList);
    }

    return polygon;
}

mapbox::geojson::polygon Polygon::convert(jni::JNIEnv &env, jni::Object<java::util::List/*<java::util::List<Point>>*/> jPointListsList) {
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


jni::Object<java::util::List> Polygon::coordinates(jni::JNIEnv &env, jni::Object<Polygon> jPolygon) {
    static auto method = Polygon::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
    return jPolygon.Call(env, method);
}

void Polygon::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<Polygon>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Polygon> Polygon::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl