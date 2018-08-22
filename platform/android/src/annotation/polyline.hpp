#pragma once

#include <mbgl/annotation/annotation.hpp>

#include "multi_point.hpp"

namespace mbgl {
namespace android {

class Polyline : private MultiPoint {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/annotations/Polyline"; };

    static mbgl::LineAnnotation toAnnotation(jni::JNIEnv&, const jni::Object<Polyline>&);

    static void registerNative(jni::JNIEnv&);
};


} // namespace android
} // namespace mbgl