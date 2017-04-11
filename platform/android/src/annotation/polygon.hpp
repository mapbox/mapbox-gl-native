#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/color.hpp>

#include <string>

#include "multi_point.hpp"

#include "../geometry/lat_lng.hpp"
#include "../java/util.hpp"

namespace mbgl {
namespace android {

class Polygon : private MultiPoint {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/annotations/Polygon"; };

    static jni::Class<Polygon> javaClass;

    static mbgl::FillAnnotation toAnnotation(jni::JNIEnv&, jni::Object<Polygon>);

    static void registerNative(jni::JNIEnv&);

private:

    static jni::Object<java::util::List> getPoints(jni::JNIEnv&, jni::Object<Polygon>);

    static jni::Object<java::util::List> getHoles(jni::JNIEnv&, jni::Object<Polygon>);

    static float getOpacity(jni::JNIEnv&, jni::Object<Polygon>);

    static mbgl::Color getFillColor(jni::JNIEnv&, jni::Object<Polygon>);

    static mbgl::Color getOutlineColor(jni::JNIEnv&, jni::Object<Polygon>);
};


} // namespace android
} // namespace mbgl