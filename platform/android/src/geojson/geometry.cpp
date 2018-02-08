#include "geometry.hpp"

#include "point.hpp"
#include "multi_point.hpp"
#include "line_string.hpp"
#include "multi_line_string.hpp"
#include "polygon.hpp"
#include "multi_polygon.hpp"

#include <string>

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::geometry Geometry::convert(jni::JNIEnv &env, jni::Object<Geometry> jGeometry) {
    auto type = Geometry::getType(env, jGeometry);
    if (type == Point::Type()) {
        return { Point::convert(env, jni::Object<Point>(jGeometry.Get())) };
    } else if (type == MultiPoint::Type()) {
        return { MultiPoint::convert(env, jni::Object<MultiPoint>(jGeometry.Get())) };
    } else if (type == LineString::Type()) {
        return { LineString::convert(env, jni::Object<LineString>(jGeometry.Get())) };
    } else if (type == MultiLineString::Type()) {
        return { MultiLineString::convert(env, jni::Object<MultiLineString>(jGeometry.Get())) };
    } else if (type == Polygon::Type()) {
        return { Polygon::convert(env, jni::Object<Polygon>(jGeometry.Get())) };
    } else if (type == MultiPolygon::Type()) {
        return { MultiPolygon::convert(env, jni::Object<MultiPolygon>(jGeometry.Get())) };
    }

    throw std::runtime_error(std::string {"Unsupported GeoJSON type: " } + type);
}

std::string Geometry::getType(jni::JNIEnv &env, jni::Object<Geometry> jGeometry) {
    static auto method = Geometry::javaClass.GetMethod<jni::String ()>(env, "type");
    auto jType = jGeometry.Call(env, method);
    auto type = jni::Make<std::string>(env, jType);
    jni::DeleteLocalRef(env, jType);
    return type;
}

void Geometry::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<Geometry>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Geometry> Geometry::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl