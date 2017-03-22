#include "multi_polygon.hpp"

#include "polygon.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::multi_polygon MultiPolygon::convert(jni::JNIEnv &env, jni::Object<MultiPolygon> jMultiPolygon) {
    mapbox::geojson::multi_polygon multiPolygon;

    if (jMultiPolygon) {
        auto jPositionListsListList = MultiPolygon::getCoordinates(env, jMultiPolygon);
        auto jPositionListsListArray = java::util::List::toArray<java::util::List>(env, jPositionListsListList);

        auto size = jPositionListsListArray.Length(env);
        multiPolygon.reserve(size);

        for (size_t i = 0; i < size; i++) {
            auto jPositionListsList = jPositionListsListArray.Get(env, i);
            multiPolygon.push_back(Polygon::convert(env, jPositionListsList));
            jni::DeleteLocalRef(env, jPositionListsList);
        }

        jni::DeleteLocalRef(env, jPositionListsListList);
        jni::DeleteLocalRef(env, jPositionListsListArray);
    }

    return multiPolygon;
}

jni::Object<java::util::List> MultiPolygon::getCoordinates(jni::JNIEnv &env, jni::Object<MultiPolygon> jPolygon) {
    static auto method = MultiPolygon::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "getCoordinates");
    return jPolygon.Call(env, method);
}

void MultiPolygon::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<MultiPolygon>::Find(env).NewGlobalRef(env).release();
}

jni::Class<MultiPolygon> MultiPolygon::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl