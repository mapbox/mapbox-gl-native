// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/layermanager/symbol_layer_factory.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class SymbolLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/SymbolLayer"; };

    SymbolLayer(jni::JNIEnv&, jni::String&, jni::String&);

    SymbolLayer(mbgl::Map&, mbgl::style::SymbolLayer&);

    SymbolLayer(mbgl::Map&, std::unique_ptr<mbgl::style::SymbolLayer>);

    ~SymbolLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getSymbolPlacement(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getSymbolSpacing(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getSymbolAvoidEdges(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getSymbolSortKey(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getSymbolZOrder(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconAllowOverlap(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconIgnorePlacement(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconOptional(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconRotationAlignment(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconSize(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconTextFit(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconTextFitPadding(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconImage(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconRotate(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconPadding(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconKeepUpright(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconOffset(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconAnchor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconPitchAlignment(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextPitchAlignment(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextRotationAlignment(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextField(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextFont(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextSize(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextMaxWidth(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextLineHeight(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextLetterSpacing(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextJustify(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextRadialOffset(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextVariableAnchor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextAnchor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextMaxAngle(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextWritingMode(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextRotate(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextPadding(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextKeepUpright(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextTransform(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextOffset(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextAllowOverlap(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextIgnorePlacement(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextOptional(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconOpacity(jni::JNIEnv&);
    void setIconOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getIconOpacityTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconColor(jni::JNIEnv&);
    void setIconColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getIconColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconHaloColor(jni::JNIEnv&);
    void setIconHaloColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getIconHaloColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconHaloWidth(jni::JNIEnv&);
    void setIconHaloWidthTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getIconHaloWidthTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconHaloBlur(jni::JNIEnv&);
    void setIconHaloBlurTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getIconHaloBlurTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconTranslate(jni::JNIEnv&);
    void setIconTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getIconTranslateTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getIconTranslateAnchor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextOpacity(jni::JNIEnv&);
    void setTextOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getTextOpacityTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextColor(jni::JNIEnv&);
    void setTextColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getTextColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextHaloColor(jni::JNIEnv&);
    void setTextHaloColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getTextHaloColorTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextHaloWidth(jni::JNIEnv&);
    void setTextHaloWidthTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getTextHaloWidthTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextHaloBlur(jni::JNIEnv&);
    void setTextHaloBlurTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getTextHaloBlurTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextTranslate(jni::JNIEnv&);
    void setTextTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getTextTranslateTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getTextTranslateAnchor(jni::JNIEnv&);

}; // class SymbolLayer

class SymbolJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::SymbolLayerFactory {
public:
    ~SymbolJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class SymbolJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
