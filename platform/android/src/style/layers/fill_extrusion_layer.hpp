// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/layermanager/fill_extrusion_layer_factory.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class FillExtrusionLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/FillExtrusionLayer"; };

    FillExtrusionLayer(jni::JNIEnv&, jni::String&, jni::String&);

    FillExtrusionLayer(mbgl::Map&, mbgl::style::FillExtrusionLayer&);

    FillExtrusionLayer(mbgl::Map&, std::unique_ptr<mbgl::style::FillExtrusionLayer>);

    ~FillExtrusionLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getFillExtrusionOpacity(jni::JNIEnv&);
    void setFillExtrusionOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillExtrusionOpacityTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillExtrusionColor(jni::JNIEnv&);
    void setFillExtrusionColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillExtrusionColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillExtrusionTranslate(jni::JNIEnv&);
    void setFillExtrusionTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillExtrusionTranslateTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillExtrusionTranslateAnchor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillExtrusionPattern(jni::JNIEnv&);
    void setFillExtrusionPatternTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillExtrusionPatternTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillExtrusionHeight(jni::JNIEnv&);
    void setFillExtrusionHeightTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillExtrusionHeightTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillExtrusionBase(jni::JNIEnv&);
    void setFillExtrusionBaseTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getFillExtrusionBaseTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getFillExtrusionVerticalGradient(jni::JNIEnv&);

}; // class FillExtrusionLayer

class FillExtrusionJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::FillExtrusionLayerFactory {
public:
    ~FillExtrusionJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class FillExtrusionJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
