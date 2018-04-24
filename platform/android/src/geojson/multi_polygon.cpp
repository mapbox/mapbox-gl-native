#include "multi_polygon.hpp"

#include "polygon.hpp"
#include "util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Object<MultiPolygon> MultiPolygon::New(JNIEnv& env, const mbgl::MultiPolygon<double>& multiPolygon) {
    auto jarray = jni::Array<jni::Object<java::util::List>>::New(env, multiPolygon.size(), java::util::List::javaClass);

    for (size_t i = 0; i < multiPolygon.size(); i++) {
        auto& geometry = multiPolygon.at(i);
        auto jPolygon = asPointsListsList(env, geometry);
        jarray.Set(env, i, jPolygon);
        jni::DeleteLocalRef(env, jPolygon);
    }

    // Turn into array list
    auto jList = java::util::Arrays::asList(env, jarray);
    jni::DeleteLocalRef(env, jarray);

    // create the MultiPolygon
    static auto method = javaClass.GetStaticMethod<jni::Object<MultiPolygon> (jni::Object<java::util::List>)>(env, "fromLngLats");
    auto jMultiPolygon = javaClass.Call(env, method, jList);

    jni::DeleteLocalRef(env, jList);
    return jMultiPolygon;
}

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