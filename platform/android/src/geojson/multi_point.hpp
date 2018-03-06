#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

#include "../java/util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

class MultiPoint : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/MultiPoint"; };

    static constexpr auto Type() { return "MultiPoint"; };

    static mapbox::geojson::multi_point convert(jni::JNIEnv&, jni::Object<MultiPoint>);

    static jni::Object<java::util::List> coordinates(jni::JNIEnv&, jni::Object<MultiPoint>);

    static jni::Class<MultiPoint> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl