#include "multi_polygon.hpp"

#include "polygon.hpp"
#include "util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Object<MultiPolygon> MultiPolygon::New(JNIEnv& env, const mbgl::MultiPolygon<double>& multiPolygon) {
    static auto javaClass = jni::Class<MultiPolygon>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<MultiPolygon> (jni::Object<java::util::List>)>(env, "fromLngLats");

    auto jarray = jni::SeizeLocal(env,
        jni::Array<jni::Object<java::util::List>>::New(env, multiPolygon.size()));

    for (size_t i = 0; i < multiPolygon.size(); i++) {
        jarray->Set(env, i, *jni::SeizeLocal(env, asPointsListsList(env, multiPolygon.at(i))));
    }

    return javaClass.Call(env, method,
        *jni::SeizeLocal(env, java::util::Arrays::asList(env, *jarray)));
}

mapbox::geojson::multi_polygon MultiPolygon::convert(jni::JNIEnv &env, jni::Object<MultiPolygon> jMultiPolygon) {
    mapbox::geojson::multi_polygon multiPolygon;

    if (jMultiPolygon) {
        auto jPointListsListList = jni::SeizeLocal(env, MultiPolygon::coordinates(env, jMultiPolygon));
        auto jPointListsListArray = jni::SeizeLocal(env, java::util::List::toArray<java::util::List>(env, *jPointListsListList));

        auto size = jPointListsListArray->Length(env);
        multiPolygon.reserve(size);

        for (size_t i = 0; i < size; i++) {
            multiPolygon.push_back(Polygon::convert(env, *jni::SeizeLocal(env, jPointListsListArray->Get(env, i))));
        }
    }

    return multiPolygon;
}

jni::Object<java::util::List> MultiPolygon::coordinates(jni::JNIEnv &env, jni::Object<MultiPolygon> jPolygon) {
    static auto javaClass = jni::Class<MultiPolygon>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
    return jPolygon.Call(env, method);
}

void MultiPolygon::registerNative(jni::JNIEnv &env) {
    jni::Class<MultiPolygon>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl