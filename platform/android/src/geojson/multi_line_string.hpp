#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include "../java/util.hpp"
#include "geometry.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class MultiLineString {
public:
    using SuperTag = Geometry;
    static constexpr auto Name() { return "com/mapbox/geojson/MultiLineString"; };
    static constexpr auto Type() { return "MultiLineString"; };

    static jni::Local<jni::Object<MultiLineString>> New(jni::JNIEnv&, const mbgl::MultiLineString<double>&);

    static mapbox::geojson::multi_line_string convert(jni::JNIEnv&, const jni::Object<MultiLineString>&);

    static mapbox::geojson::multi_line_string convert(jni::JNIEnv&, const jni::Object<java::util::List/*<java::util::List<Point>>*/>&);

    static jni::Local<jni::Object<java::util::List>> coordinates(jni::JNIEnv&, const jni::Object<MultiLineString>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl