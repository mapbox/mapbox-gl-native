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

class MultiPoint : public Geometry {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/MultiPoint"; };

    static constexpr auto Type() { return "MultiPoint"; };

    static jni::Object<MultiPoint> New(jni::JNIEnv&, const mbgl::MultiPoint<double>&);

    static mapbox::geojson::multi_point convert(jni::JNIEnv&, jni::Object<MultiPoint>);

    static jni::Object<java::util::List> coordinates(jni::JNIEnv&, jni::Object<MultiPoint>);

    static jni::Class<MultiPoint> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl