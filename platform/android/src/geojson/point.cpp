#include "point.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Local<jni::Object<Point>> Point::New(jni::JNIEnv& env, const mbgl::Point<double>& point) {
    static auto& javaClass = jni::Class<Point>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<Point> (jni::jdouble, jni::jdouble)>(env, "fromLngLat");
    return javaClass.Call(env, method, point.x, point.y);
}

mbgl::Point<double> Point::convert(jni::JNIEnv &env, const jni::Object<Point>& jPoint) {
    static auto& javaClass = jni::Class<Point>::Singleton(env);
    static auto longitude = javaClass.GetMethod<jni::jdouble ()>(env, "longitude");
    static auto latitude = javaClass.GetMethod<jni::jdouble ()>(env, "latitude");

    if (!jPoint) {
        return {};
    }

    return {
        jPoint.Call(env, longitude),
        jPoint.Call(env, latitude)
    };
}

void Point::registerNative(jni::JNIEnv &env) {
    jni::Class<Point>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl
