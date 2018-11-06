#include "geometry.hpp"

#include "point.hpp"
#include "multi_point.hpp"
#include "line_string.hpp"
#include "multi_line_string.hpp"
#include "polygon.hpp"
#include "multi_polygon.hpp"
#include "geometry_collection.hpp"

#include <string>

namespace mbgl {
namespace android {
namespace geojson {

/**
 * Turn mapbox::geometry type into Java GeoJson Geometries
 */
class GeometryEvaluator {
public:

    jni::JNIEnv& env;

    jni::Local<jni::Object<Geometry>> operator()(const mbgl::EmptyGeometry &) const {
        // FIXME: mapbox-java needs to have its own Empty type.
        return GeometryCollection::New(env, {});
    }

    jni::Local<jni::Object<Geometry>> operator()(const mbgl::Point<double> &geometry) const {
        return Point::New(env, geometry);
    }

    jni::Local<jni::Object<Geometry>> operator()(const mbgl::LineString<double> &geometry) const {
        return LineString::New(env, geometry);
    }

    jni::Local<jni::Object<Geometry>>  operator()(const mbgl::MultiLineString<double> &geometry) const {
        return MultiLineString::New(env, geometry);
    }

    jni::Local<jni::Object<Geometry>> operator()(const mbgl::MultiPoint<double> &geometry) const {
        return MultiPoint::New(env, geometry);
    }

    jni::Local<jni::Object<Geometry>> operator()(const mbgl::Polygon<double> &geometry) const {
        return Polygon::New(env, geometry);
    }

    jni::Local<jni::Object<Geometry>>  operator()(const mbgl::MultiPolygon<double> &geometry) const {
        return MultiPolygon::New(env, geometry);
    }

    jni::Local<jni::Object<Geometry>>  operator()(const mapbox::geometry::geometry_collection<double> &geometry) const {
        return GeometryCollection::New(env, geometry);
    }
};

jni::Local<jni::Object<Geometry>> Geometry::New(jni::JNIEnv& env, mbgl::Geometry<double> geometry) {
    GeometryEvaluator evaluator { env } ;
    return mbgl::Geometry<double>::visit(geometry, evaluator);
}

mbgl::Geometry<double> Geometry::convert(jni::JNIEnv &env, const jni::Object<Geometry>& jGeometry) {
    auto type = Geometry::getType(env, jGeometry);
    if (type == Point::Type()) {
        return { Point::convert(env, jni::Cast(env, jni::Class<Point>::Singleton(env), jGeometry)) };
    } else if (type == MultiPoint::Type()) {
        return { MultiPoint::convert(env, jni::Cast(env, jni::Class<MultiPoint>::Singleton(env), jGeometry)) };
    } else if (type == LineString::Type()) {
        return { LineString::convert(env, jni::Cast(env, jni::Class<LineString>::Singleton(env), jGeometry)) };
    } else if (type == MultiLineString::Type()) {
        return { MultiLineString::convert(env, jni::Cast(env, jni::Class<MultiLineString>::Singleton(env), jGeometry)) };
    } else if (type == Polygon::Type()) {
        return { Polygon::convert(env, jni::Cast(env, jni::Class<Polygon>::Singleton(env), jGeometry)) };
    } else if (type == MultiPolygon::Type()) {
        return { MultiPolygon::convert(env, jni::Cast(env, jni::Class<MultiPolygon>::Singleton(env), jGeometry)) };
    } else if (type == GeometryCollection::Type()) {
        return { GeometryCollection::convert(env, jni::Cast(env, jni::Class<GeometryCollection>::Singleton(env), jGeometry)) };
    }

    throw std::runtime_error(std::string {"Unsupported GeoJSON type: " } + type);
}

std::string Geometry::getType(jni::JNIEnv &env, const jni::Object<Geometry>& jGeometry) {
    static auto& javaClass = jni::Class<Geometry>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::String ()>(env, "type");
    return jni::Make<std::string>(env, jGeometry.Call(env, method));
}

void Geometry::registerNative(jni::JNIEnv &env) {
    jni::Class<Geometry>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl
