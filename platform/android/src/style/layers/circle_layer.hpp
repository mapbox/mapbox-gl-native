// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/layermanager/circle_layer_factory.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class CircleLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/CircleLayer"; };

    CircleLayer(jni::JNIEnv&, jni::String&, jni::String&);

    CircleLayer(mbgl::Map&, mbgl::style::CircleLayer&);

    CircleLayer(mbgl::Map&, std::unique_ptr<mbgl::style::CircleLayer>);

    ~CircleLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getCircleRadius(jni::JNIEnv&);
    void setCircleRadiusTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getCircleRadiusTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCircleColor(jni::JNIEnv&);
    void setCircleColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getCircleColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCircleBlur(jni::JNIEnv&);
    void setCircleBlurTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getCircleBlurTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCircleOpacity(jni::JNIEnv&);
    void setCircleOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getCircleOpacityTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCircleTranslate(jni::JNIEnv&);
    void setCircleTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getCircleTranslateTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCircleTranslateAnchor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCirclePitchScale(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCirclePitchAlignment(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCircleStrokeWidth(jni::JNIEnv&);
    void setCircleStrokeWidthTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getCircleStrokeWidthTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCircleStrokeColor(jni::JNIEnv&);
    void setCircleStrokeColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getCircleStrokeColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getCircleStrokeOpacity(jni::JNIEnv&);
    void setCircleStrokeOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getCircleStrokeOpacityTransition(jni::JNIEnv&);

}; // class CircleLayer

class CircleJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::CircleLayerFactory {
public:
    ~CircleJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class CircleJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
