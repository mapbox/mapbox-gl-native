#include "multi_point.hpp"

#include "line_string.hpp"

#include "util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Object<MultiPoint> MultiPoint::New(JNIEnv& env, const mbgl::MultiPoint<double>& multiPoint) {
    static auto javaClass = jni::Class<MultiPoint>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<MultiPoint>(jni::Object<java::util::List>)>(env, "fromLngLats");

    return javaClass.Call(env, method, *jni::SeizeLocal(env, asPointsList(env, multiPoint)));
}

mapbox::geojson::multi_point MultiPoint::convert(jni::JNIEnv &env, jni::Object<MultiPoint> jMultiPoint) {
    mapbox::geojson::multi_point multiPoint;

    if (jMultiPoint) {
        multiPoint = convertExplicit<mapbox::geojson::multi_point>(
            LineString::convert(env, *jni::SeizeLocal(env, MultiPoint::coordinates(env, jMultiPoint))));
    }

    return multiPoint;
}

jni::Object<java::util::List> MultiPoint::coordinates(jni::JNIEnv &env, jni::Object<MultiPoint> jMultiPoint) {
    static auto javaClass = jni::Class<MultiPoint>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
    return jMultiPoint.Call(env, method);
}

void MultiPoint::registerNative(jni::JNIEnv &env) {
    jni::Class<MultiPoint>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl