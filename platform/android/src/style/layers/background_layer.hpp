// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/layermanager/background_layer_factory.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class BackgroundLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/BackgroundLayer"; };

    BackgroundLayer(jni::JNIEnv&, jni::String&);

    BackgroundLayer(mbgl::Map&, mbgl::style::BackgroundLayer&);

    BackgroundLayer(mbgl::Map&, std::unique_ptr<mbgl::style::BackgroundLayer>);

    ~BackgroundLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getBackgroundColor(jni::JNIEnv&);
    void setBackgroundColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getBackgroundColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getBackgroundPattern(jni::JNIEnv&);
    void setBackgroundPatternTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getBackgroundPatternTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getBackgroundOpacity(jni::JNIEnv&);
    void setBackgroundOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getBackgroundOpacityTransition(jni::JNIEnv&);

}; // class BackgroundLayer

class BackgroundJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::BackgroundLayerFactory {
public:
    ~BackgroundJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class BackgroundJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
