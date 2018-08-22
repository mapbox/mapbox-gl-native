#include "geometry_collection.hpp"
#include "../java/util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Local<jni::Object<GeometryCollection>> GeometryCollection::New(jni::JNIEnv& env, const mapbox::geometry::geometry_collection<double>& collection) {
    // Create an array of geometries
    auto jarray = jni::Array<jni::Object<Geometry>>::New(env, collection.size());

    for (size_t i = 0; i < collection.size(); i++) {
        jarray.Set(env, i, Geometry::New(env, collection.at(i)));
    }

    // create the GeometryCollection
    static auto& javaClass = jni::Class<GeometryCollection>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<GeometryCollection> (jni::Object<java::util::List>)>(env, "fromGeometries");
    return javaClass.Call(env, method, java::util::Arrays::asList(env, jarray));
}

mapbox::geometry::geometry_collection<double> GeometryCollection::convert(jni::JNIEnv &env, const jni::Object<GeometryCollection>& jCollection) {
    // Get geometries
    static auto& javaClass = jni::Class<GeometryCollection>::Singleton(env);
    static auto getGeometries = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "geometries");

    // Turn into array
    auto jarray = java::util::List::toArray<Geometry>(env, jCollection.Call(env, getGeometries));

    // Convert each geometry
    mapbox::geometry::geometry_collection<double> collection{};

    auto size = jarray.Length(env);
    for (jni::jsize i = 0; i < size; i++) {
        collection.push_back(Geometry::convert(env, jarray.Get(env, i)));
    }

    return collection;
}

void GeometryCollection::registerNative(jni::JNIEnv &env) {
    jni::Class<GeometryCollection>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl