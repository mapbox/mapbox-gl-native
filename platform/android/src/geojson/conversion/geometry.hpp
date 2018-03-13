#pragma once

#include <mapbox/geometry.hpp>
#include "../geometry.hpp"
#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace conversion {

/**
 * mapbox::geometry::geometry<T> -> Java GeoJson Geometry
 */
template <class T>
struct Converter<jni::Object<android::geojson::Geometry>, mapbox::geometry::geometry<T>> {
    Result<jni::Object<android::geojson::Geometry>> operator()(jni::JNIEnv& env, const mapbox::geometry::geometry<T>& value) const {
       return { android::geojson::Geometry::New(env, value) };
    }
};


} // conversion
} // android
} // mbgl
