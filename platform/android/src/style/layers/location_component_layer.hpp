// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/layermanager/location_component_layer_factory.hpp>
#include <mbgl/style/layers/location_component_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class LocationComponentLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/LocationComponentLayer"; };

    LocationComponentLayer(jni::JNIEnv&, jni::String&);

    LocationComponentLayer(mbgl::style::LocationComponentLayer&);

    LocationComponentLayer(std::unique_ptr<mbgl::style::LocationComponentLayer>);

    ~LocationComponentLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getTopImage(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getBearingImage(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getShadowImage(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getLocation(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getBearing(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getAccuracyRadius(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTopImageSize(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getBearingImageSize(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getShadowImageSize(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getPerspectiveCompensation(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getImageTiltDisplacement(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getAccuracyRadiusColor(jni::JNIEnv&);
    void setAccuracyRadiusColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getAccuracyRadiusColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getAccuracyRadiusBorderColor(jni::JNIEnv&);
    void setAccuracyRadiusBorderColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getAccuracyRadiusBorderColorTransition(jni::JNIEnv&);

}; // class LocationComponentLayer

class LocationComponentJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::LocationComponentLayerFactory {
public:
    ~LocationComponentJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class LocationComponentJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
