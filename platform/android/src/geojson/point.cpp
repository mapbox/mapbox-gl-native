#include "point.hpp"
#include "../java/util.hpp"
#include "../java_types.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::point Point::convert(jni::JNIEnv &env, jni::Object<Point> jPoint) {
    mapbox::geojson::point point;

    if (jPoint) {
        auto jDoubleList = Point::coordinates(env, jPoint);
        point = Point::convert(env, jDoubleList);
        jni::DeleteLocalRef(env, jDoubleList);
    }

    return point;
}

mapbox::geojson::point Point::convert(jni::JNIEnv &env, jni::Object<java::util::List/*<Double>*/> jDoubleList) {
    auto jDoubleArray = java::util::List::toArray<double>(env, jDoubleList);

    jni::jdouble lon = jni::CallMethod<jni::jdouble>(env,
                                                     jDoubleArray.Get(env, 0),
                                                     *java::Number::doubleValueMethodId);
    jni::jdouble lat = jni::CallMethod<jni::jdouble>(env,
                                                      jDoubleArray.Get(env, 1),
                                                      *java::Number::doubleValueMethodId);
    mapbox::geojson::point point(lon, lat);
    jni::DeleteLocalRef(env, jDoubleArray);

    return point;
}

jni::Object<java::util::List> Point::coordinates(jni::JNIEnv &env, jni::Object<Point> jPoint) {
     static auto method = Point::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
     return jPoint.Call(env, method);
}

void Point::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<Point>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Point> Point::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl