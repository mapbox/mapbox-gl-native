#include "geometry_collection.hpp"
#include "../java/util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Object<GeometryCollection> GeometryCollection::New(jni::JNIEnv& env, const mapbox::geometry::geometry_collection<double>& collection) {
    // Create an array of geometries
    auto jarray = jni::Array<jni::Object<Geometry>>::New(env, collection.size(), Geometry::javaClass);

    for (size_t i = 0; i < collection.size(); i++) {
        auto& geometry = collection.at(i);
        auto jGeometry = Geometry::New(env, geometry);
        jarray.Set(env, i, jGeometry);
        jni::DeleteLocalRef(env, jGeometry);
    }

    // Turn into array list
    auto jList = java::util::Arrays::asList(env, jarray);
    jni::DeleteLocalRef(env, jarray);

    // create the GeometryCollection
    static auto method = javaClass.GetStaticMethod<jni::Object<GeometryCollection> (jni::Object<java::util::List>)>(env, "fromGeometries");
    auto jCollection = javaClass.Call(env, method, jList);

    jni::DeleteLocalRef(env, jList);
    return jCollection;
}

mapbox::geometry::geometry_collection<double> GeometryCollection::convert(jni::JNIEnv &env, jni::Object<GeometryCollection> jCollection) {
    // Get geometries
    static auto getGeometries = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "getGeometries");
    auto jList = jCollection.Call(env, getGeometries);

    // Turn into array
    auto jarray = java::util::List::toArray<Geometry>(env, jList);
    jni::DeleteLocalRef(env, jList);

    // Convert each geometry
    mapbox::geometry::geometry_collection<double> collection{};

    auto size = jarray.Length(env);
    for (jni::jsize i = 0; i < size; i++) {
        auto element = jarray.Get(env, i);
        collection.push_back(Geometry::convert(env, element));
        jni::DeleteLocalRef(env, element);
    }

    jni::DeleteLocalRef(env, jarray);
    return collection;
}

void GeometryCollection::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<GeometryCollection>::Find(env).NewGlobalRef(env).release();
}

jni::Class<GeometryCollection> GeometryCollection::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl