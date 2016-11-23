// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include <mbgl/style/layers/raster_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class RasterLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/RasterLayer"; };

    static jni::Class<RasterLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    RasterLayer(jni::JNIEnv&, jni::String, jni::String);

    RasterLayer(mbgl::Map&, mbgl::style::RasterLayer&);

    ~RasterLayer();

    // Property getters
    jni::Object<jni::ObjectTag> getRasterOpacity(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterHueRotate(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterBrightnessMin(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterBrightnessMax(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterSaturation(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterContrast(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterFadeDuration(jni::JNIEnv&);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class RasterLayer

} // namespace android
} // namespace mbgl
