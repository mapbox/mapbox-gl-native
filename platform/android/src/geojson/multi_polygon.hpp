#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include "../java/util.hpp"
#include "geometry.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class MultiPolygon {
public:
    using SuperTag = Geometry;
    static constexpr auto Name() { return "com/mapbox/geojson/MultiPolygon"; };
    static constexpr auto Type() { return "MultiPolygon"; };

    static jni::Local<jni::Object<MultiPolygon>> New(jni::JNIEnv&, const mbgl::MultiPolygon<double>&);

    static mapbox::geojson::multi_polygon convert(jni::JNIEnv&, const jni::Object<MultiPolygon>&);

    static jni::Local<jni::Object<java::util::List>> coordinates(jni::JNIEnv&, const jni::Object<MultiPolygon>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl