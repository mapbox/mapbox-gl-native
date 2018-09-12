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
static jni::Local<jni::Object<java::util::List>> asPointsList(jni::JNIEnv& env, const T& pointsList) {
    auto jarray = jni::Array<jni::Object<Point>>::New(env, pointsList.size());

    for (jni::jsize i = 0; i < pointsList.size(); i++) {
        jarray.Set(env, i, Point::New(env, pointsList.at(i)));
    }

    return java::util::Arrays::asList(env, jarray);
}

/**
 *  Geometry -> List<List<Point>>
 */
template <class SHAPE>
static jni::Local<jni::Object<java::util::List>> asPointsListsList(JNIEnv& env, const SHAPE& value) {
    auto jarray = jni::Array<jni::Object<java::util::List>>::New(env, value.size());

    for (size_t i = 0; i < value.size(); i++) {
        jarray.Set(env, i, asPointsList(env, value[i]));
    }

    return java::util::Arrays::asList(env, jarray);
}

} // namespace geojson
} // namespace android
} // namespace mbgl
