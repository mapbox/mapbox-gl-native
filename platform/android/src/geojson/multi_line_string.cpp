#include "multi_line_string.hpp"

#include "line_string.hpp"
#include "util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

jni::Local<jni::Object<MultiLineString>> MultiLineString::New(jni::JNIEnv& env, const mbgl::MultiLineString<double>& multiLineString) {
    static auto& javaClass = jni::Class<MultiLineString>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<MultiLineString> (jni::Object<java::util::List>)>(env, "fromLngLats");

    return javaClass.Call(env, method, asPointsListsList(env, multiLineString));
}

mapbox::geojson::multi_line_string MultiLineString::convert(jni::JNIEnv &env, const jni::Object<MultiLineString>& jMultiLineString) {
    mapbox::geojson::multi_line_string multiLineString;

    if (jMultiLineString) {
        multiLineString = MultiLineString::convert(env, MultiLineString::coordinates(env, jMultiLineString));
    }

    return multiLineString;
}

mapbox::geojson::multi_line_string MultiLineString::convert(jni::JNIEnv &env, const jni::Object<java::util::List/*<java::util::List<Point>>*/>& jPointListsList) {
    mapbox::geojson::multi_line_string multiLineString;

    if (jPointListsList) {
        auto jPositionListsArray = java::util::List::toArray<java::util::List>(env, jPointListsList);

        auto size = jPositionListsArray.Length(env);
        multiLineString.reserve(size);

        for (std::size_t i = 0; i < size; i++) {
            multiLineString.push_back(LineString::convert(env, jPositionListsArray.Get(env, i)));
        }
    }

    return multiLineString;
}

jni::Local<jni::Object<java::util::List>> MultiLineString::coordinates(jni::JNIEnv &env, const jni::Object<MultiLineString>& jLineString) {
    static auto& javaClass = jni::Class<MultiLineString>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "coordinates");
    return jLineString.Call(env, method);
}

void MultiLineString::registerNative(jni::JNIEnv &env) {
    jni::Class<MultiLineString>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl