#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/noncopyable.hpp>

#include "geometry.hpp"
#include "../java/util.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class MultiPoint {
public:
    using SuperTag = Geometry;
    static constexpr auto Name() { return "com/mapbox/geojson/MultiPoint"; };
    static constexpr auto Type() { return "MultiPoint"; };

    static jni::Local<jni::Object<MultiPoint>> New(jni::JNIEnv&, const mbgl::MultiPoint<double>&);

    static mapbox::geojson::multi_point convert(jni::JNIEnv&, const jni::Object<MultiPoint>&);

    static jni::Local<jni::Object<java::util::List>> coordinates(jni::JNIEnv&, const jni::Object<MultiPoint>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl