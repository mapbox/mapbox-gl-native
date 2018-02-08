#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

#include "../java/util.hpp"

namespace mbgl {
namespace android {
namespace geojson {

class MultiPolygon : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/MultiPolygon"; };

    static constexpr auto Type() { return "MultiPolygon"; };

    static mapbox::geojson::multi_polygon convert(jni::JNIEnv&, jni::Object<MultiPolygon>);

    static jni::Object<java::util::List> coordinates(jni::JNIEnv&, jni::Object<MultiPolygon>);

    static jni::Class<MultiPolygon> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl