#include "multi_line_string.hpp"

#include "line_string.hpp"

namespace mbgl {
namespace android {
namespace geojson {

mapbox::geojson::multi_line_string MultiLineString::convert(jni::JNIEnv &env, jni::Object<MultiLineString> jMultiLineString) {
    mapbox::geojson::multi_line_string multiLineString;

    if (jMultiLineString) {
        auto jPositionListsList = MultiLineString::getCoordinates(env, jMultiLineString);
        multiLineString = MultiLineString::convert(env, jPositionListsList);
        jni::DeleteLocalRef(env, jPositionListsList);
    }

    return multiLineString;
}

mapbox::geojson::multi_line_string MultiLineString::convert(jni::JNIEnv &env, jni::Object<java::util::List/*<java::util::List<Position>>*/> jPositionListsList) {
    mapbox::geojson::multi_line_string multiLineString;

    if (jPositionListsList) {
        auto jPositionListsArray = java::util::List::toArray<java::util::List>(env, jPositionListsList);

        auto size = jPositionListsArray.Length(env);
        multiLineString.reserve(size);

        for (std::size_t i = 0; i < size; i++) {
            auto jPositionList = jPositionListsArray.Get(env, i);
            multiLineString.push_back(LineString::convert(env, jPositionList));
            jni::DeleteLocalRef(env, jPositionList);
        }

        jni::DeleteLocalRef(env, jPositionListsArray);
    }

    return multiLineString;
}

jni::Object<java::util::List> MultiLineString::getCoordinates(jni::JNIEnv &env, jni::Object<MultiLineString> jLineString) {
    static auto method = MultiLineString::javaClass.GetMethod<jni::Object<java::util::List> ()>(env, "getCoordinates");
    return jLineString.Call(env, method);
}

void MultiLineString::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<MultiLineString>::Find(env).NewGlobalRef(env).release();
}

jni::Class<MultiLineString> MultiLineString::javaClass;

} // namespace geojson
} // namespace android
} // namespace mbgl