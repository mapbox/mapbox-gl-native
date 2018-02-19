#include "multi_polygon.hpp"

#include "polygon.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::multi_polygon MultiPolygon::convert(jni::JNIEnv &env, jni::Object<MultiPolygon> jMultiPolygon) {
    mapbox::geojson::multi_polygon multiPolygon;

    if (jMultiPolygon) {
        auto jPointListsListList = MultiPolygon::coordinates(env, jMultiPolygon);
        auto jPointListsListArray = java::util::List::toArray<java::util::List>(env, jPointListsListList);

        auto size = jPointListsListArray.Length(env);
        multiPolygon.reserve(size);

        for (size_t i = 0; i < size; i++) {
            auto jPositionListsList = jPointListsListArray.Get(env, i);
            multiPolygon.push_back(Polygon::convert(env, jPositionListsList));
            jni::DeleteLocalRef(env, jPositionListsList);
        }

        jni::DeleteLocalRef(env, jPointListsListArray);
        jni::DeleteLocalRef(env, jPointListsListList);
    }

    return multiPolygon;
}

jni::Object<java::util::List> MultiPolygon::coordinates(jni::JNIEnv &env, jni::Object<MultiPolygon> jPolygon) {
    static auto method = MultiPolygon::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
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