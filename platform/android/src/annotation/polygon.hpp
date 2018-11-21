#pragma once

#include <mbgl/annotation/annotation.hpp>

#include "multi_point.hpp"

namespace mbgl {
namespace android {

class Polygon : private MultiPoint {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/annotations/Polygon"; };

    static mbgl::FillAnnotation toAnnotation(jni::JNIEnv&, const jni::Object<Polygon>&);

    static void registerNative(jni::JNIEnv&);
};


} // namespace android
} // namespace mbgl