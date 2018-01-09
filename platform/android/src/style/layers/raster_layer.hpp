// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
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

    RasterLayer(mbgl::Map&, std::unique_ptr<mbgl::style::RasterLayer>);

    ~RasterLayer();

    // Properties

    jni::Object<jni::ObjectTag> getRasterOpacity(jni::JNIEnv&);
    void setRasterOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getRasterOpacityTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterHueRotate(jni::JNIEnv&);
    void setRasterHueRotateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getRasterHueRotateTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterBrightnessMin(jni::JNIEnv&);
    void setRasterBrightnessMinTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getRasterBrightnessMinTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterBrightnessMax(jni::JNIEnv&);
    void setRasterBrightnessMaxTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getRasterBrightnessMaxTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterSaturation(jni::JNIEnv&);
    void setRasterSaturationTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getRasterSaturationTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterContrast(jni::JNIEnv&);
    void setRasterContrastTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getRasterContrastTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getRasterFadeDuration(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class RasterLayer

} // namespace android
} // namespace mbgl
