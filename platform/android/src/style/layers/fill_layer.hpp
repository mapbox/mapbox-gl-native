// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/layermanager/fill_layer_factory.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class FillLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/FillLayer"; };

    FillLayer(jni::JNIEnv&, jni::String&, jni::String&);

    FillLayer(mbgl::Map&, mbgl::style::FillLayer&);

    FillLayer(mbgl::Map&, std::unique_ptr<mbgl::style::FillLayer>);

    ~FillLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getFillAntialias(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillOpacity(jni::JNIEnv&);
    void setFillOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillOpacityTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillColor(jni::JNIEnv&);
    void setFillColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillOutlineColor(jni::JNIEnv&);
    void setFillOutlineColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillOutlineColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillTranslate(jni::JNIEnv&);
    void setFillTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillTranslateTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillTranslateAnchor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillPattern(jni::JNIEnv&);
    void setFillPatternTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillPatternTransition(jni::JNIEnv&);

}; // class FillLayer

class FillJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::FillLayerFactory {
public:
    ~FillJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class FillJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
