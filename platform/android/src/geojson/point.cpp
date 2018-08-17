#include "point.hpp"
#include "../java/util.hpp"
#include "../java_types.hpp"
#include "../style/value.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Object<Point> Point::New(jni::JNIEnv& env, const mbgl::Point<double>& point) {
    static auto javaClass = jni::Class<Point>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<Point> (jni::jdouble, jni::jdouble)>(env, "fromLngLat");
    return javaClass.Call(env, method, point.x, point.y);
}

mapbox::geojson::point Point::convert(jni::JNIEnv &env, jni::Object<Point> jPoint) {
    mapbox::geojson::point point;

    if (jPoint) {
        point = Point::convert(env, *jni::SeizeLocal(env, Point::coordinates(env, jPoint)));
    }

    return point;
}

mapbox::geojson::point Point::convert(jni::JNIEnv &env, jni::Object<java::util::List/*<Double>*/> jDoubleList) {
    mapbox::geojson::point point;

    if (jDoubleList) {
        auto jDoubleArray = jni::SeizeLocal(env, java::util::List::toArray<jobject>(env, jDoubleList));

        auto lonObject = jni::SeizeLocal(env, jDoubleArray->Get(env, 0));
        auto latObject = jni::SeizeLocal(env, jDoubleArray->Get(env, 1));

        point.x = jni::CallMethod<jni::jdouble>(env, *lonObject,
                                                *java::Number::doubleValueMethodId);
        point.y = jni::CallMethod<jni::jdouble>(env, *latObject,
                                                *java::Number::doubleValueMethodId);
    }
    return point;
}

jni::Object<java::util::List> Point::coordinates(jni::JNIEnv &env, jni::Object<Point> jPoint) {
    static auto javaClass = jni::Class<Point>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
    return jPoint.Call(env, method);
}

void Point::registerNative(jni::JNIEnv &env) {
    jni::Class<Point>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl
