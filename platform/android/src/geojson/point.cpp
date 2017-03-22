#include "point.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::point Point::convert(jni::JNIEnv &env, jni::Object<Point> jPoint) {
    auto jPosition = Point::getPosition(env, jPoint);
    auto point = Position::convert(env, jPosition);
    jni::DeleteLocalRef(env, jPosition);
    return point;
}

jni::Object<Position> Point::getPosition(JNIEnv& env, jni::Object<Point> jPoint) {
    static auto method = Point::javaClass.GetMethod<jni::Object<Position> ()>(env, "getCoordinates");
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