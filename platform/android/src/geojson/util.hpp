#pragma once

#include "point.hpp"

#include <type_traits>

namespace mbgl {
namespace android {
namespace geojson {

// Clang 3.8 fails to implicitly convert matching types, so we'll have to do it explicitly.
template <typename To, typename From>
To convertExplicit(From&& src) {
    static_assert(std::is_same<typename std::decay_t<From>::container_type,
                               typename To::container_type>::value,
                  "container types do not match");
    static_assert(std::is_rvalue_reference<From&&>::value,
                  "argument must be rvalue reference");
    return *reinterpret_cast<std::add_pointer_t<To>>(&src);
}

/**
 *  Geometry -> List<Point>
 */
template <class T>
static jni::Object<java::util::List> asPointsList(jni::JNIEnv& env, const T& pointsList) {
    auto jarray = jni::Array<jni::Object<Point>>::New(env, pointsList.size(), Point::javaClass);

    for (jni::jsize i = 0; i < pointsList.size(); i++) {
        auto jPoint = Point::New(env, pointsList.at(i));
        jarray.Set(env, i, jPoint);
        jni::DeleteLocalRef(env, jPoint);
    }

    auto jList = java::util::Arrays::asList(env, jarray);
    jni::DeleteLocalRef(env, jarray);
    return jList;
}

/**
 *  Geometry -> List<List<Point>>
 */
template <class SHAPE>
static jni::Object<java::util::List> asPointsListsList(JNIEnv& env, SHAPE value) {
    auto jarray = jni::Array<jni::Object<java::util::List>>::New(env, value.size(), java::util::List::javaClass);

    for (size_t i = 0; i < value.size(); i = i + 1) {
        auto pointsList = asPointsList(env, value[i]);
        jarray.Set(env, i, pointsList);
        jni::DeleteLocalRef(env, pointsList);
    }

    auto jList = java::util::Arrays::asList(env, jarray);
    jni::DeleteLocalRef(env, jarray);
    return jList;
}

} // namespace geojson
} // namespace android
} // namespace mbgl
