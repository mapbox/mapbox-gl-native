// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/layermanager/raster_layer_factory.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class RasterLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/RasterLayer"; };

    RasterLayer(jni::JNIEnv&, jni::String&, jni::String&);

    RasterLayer(mbgl::Map&, mbgl::style::RasterLayer&);

    RasterLayer(mbgl::Map&, std::unique_ptr<mbgl::style::RasterLayer>);

    ~RasterLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getRasterOpacity(jni::JNIEnv&);
    void setRasterOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getRasterOpacityTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getRasterHueRotate(jni::JNIEnv&);
    void setRasterHueRotateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getRasterHueRotateTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getRasterBrightnessMin(jni::JNIEnv&);
    void setRasterBrightnessMinTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getRasterBrightnessMinTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getRasterBrightnessMax(jni::JNIEnv&);
    void setRasterBrightnessMaxTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getRasterBrightnessMaxTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getRasterSaturation(jni::JNIEnv&);
    void setRasterSaturationTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getRasterSaturationTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getRasterContrast(jni::JNIEnv&);
    void setRasterContrastTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getRasterContrastTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getRasterResampling(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getRasterFadeDuration(jni::JNIEnv&);

}; // class RasterLayer

class RasterJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::RasterLayerFactory {
public:
    ~RasterJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class RasterJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
