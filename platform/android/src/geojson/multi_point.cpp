#include "multi_point.hpp"

#include "line_string.hpp"

#include "util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Object<MultiPoint> MultiPoint::New(JNIEnv& env, const mbgl::MultiPoint<double>& multiPoint) {
    auto jList = asPointsList(env, multiPoint);

    static auto method = javaClass.GetStaticMethod<jni::Object<MultiPoint>(jni::Object<java::util::List>)>(env, "fromLngLats");
    auto jMultiPoint = javaClass.Call(env, method, jList);

    jni::DeleteLocalRef(env, jList);
    return jMultiPoint;
}

mapbox::geojson::multi_point MultiPoint::convert(jni::JNIEnv &env, jni::Object<MultiPoint> jMultiPoint) {
    mapbox::geojson::multi_point multiPoint;

    if (jMultiPoint) {
        auto jPointListsList = MultiPoint::coordinates(env, jMultiPoint);
        multiPoint = convertExplicit<mapbox::geojson::multi_point>(LineString::convert(env, jPointListsList));
        jni::DeleteLocalRef(env, jPointListsList);
    }

    return multiPoint;
}

jni::Object<java::util::List> MultiPoint::coordinates(jni::JNIEnv &env, jni::Object<MultiPoint> jMultiPoint) {
    static auto method = MultiPoint::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
    return jMultiPoint.Call(env, method);
}

void MultiPoint::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<MultiPoint>::Find(env).NewGlobalRef(env).release();
}

jni::Class<MultiPoint> MultiPoint::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl