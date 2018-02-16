// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class HeatmapLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/HeatmapLayer"; };

    static jni::Class<HeatmapLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    HeatmapLayer(jni::JNIEnv&, jni::String, jni::String);

    HeatmapLayer(mbgl::Map&, mbgl::style::HeatmapLayer&);

    HeatmapLayer(mbgl::Map&, std::unique_ptr<mbgl::style::HeatmapLayer>);

    ~HeatmapLayer();

    // Properties

    jni::Object<jni::ObjectTag> getHeatmapRadius(jni::JNIEnv&);
    void setHeatmapRadiusTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getHeatmapRadiusTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getHeatmapWeight(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getHeatmapIntensity(jni::JNIEnv&);
    void setHeatmapIntensityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getHeatmapIntensityTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getHeatmapOpacity(jni::JNIEnv&);
    void setHeatmapOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getHeatmapOpacityTransition(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class HeatmapLayer

} // namespace android
} // namespace mbgl
