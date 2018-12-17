// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/layermanager/hillshade_layer_factory.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class HillshadeLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/HillshadeLayer"; };

    HillshadeLayer(jni::JNIEnv&, jni::String&, jni::String&);

    HillshadeLayer(mbgl::Map&, mbgl::style::HillshadeLayer&);

    HillshadeLayer(mbgl::Map&, std::unique_ptr<mbgl::style::HillshadeLayer>);

    ~HillshadeLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getHillshadeIlluminationDirection(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHillshadeIlluminationAnchor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHillshadeExaggeration(jni::JNIEnv&);
    void setHillshadeExaggerationTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getHillshadeExaggerationTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHillshadeShadowColor(jni::JNIEnv&);
    void setHillshadeShadowColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getHillshadeShadowColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHillshadeHighlightColor(jni::JNIEnv&);
    void setHillshadeHighlightColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getHillshadeHighlightColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHillshadeAccentColor(jni::JNIEnv&);
    void setHillshadeAccentColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getHillshadeAccentColorTransition(jni::JNIEnv&);

}; // class HillshadeLayer

class HillshadeJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::HillshadeLayerFactory {
public:
    ~HillshadeJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class HillshadeJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
