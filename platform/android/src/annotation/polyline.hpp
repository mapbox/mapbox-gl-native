#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/color.hpp>

#include <string>

#include "multi_point.hpp"

#include "../geometry/lat_lng.hpp"
#include "../java/util.hpp"

namespace mbgl {
namespace android {

class Polyline : private MultiPoint {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/annotations/Polyline"; };

    static jni::Class<Polyline> javaClass;

    static mbgl::LineAnnotation toAnnotation(jni::JNIEnv&, jni::Object<Polyline>);

    static void registerNative(jni::JNIEnv&);

private:

    static jni::Object<java::util::List> getPoints(jni::JNIEnv&, jni::Object<Polyline>);

    static float getOpacity(jni::JNIEnv&, jni::Object<Polyline>);

    static mbgl::Color getColor(jni::JNIEnv&, jni::Object<Polyline>);

    static float getWidth(jni::JNIEnv&, jni::Object<Polyline>);
};


} // namespace android
} // namespace mbgl