// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/symbol_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class SymbolLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/SymbolLayer"; };

    static jni::Class<SymbolLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    SymbolLayer(jni::JNIEnv&, jni::String, jni::String);

    SymbolLayer(mbgl::Map&, mbgl::style::SymbolLayer&);

    SymbolLayer(mbgl::Map&, std::unique_ptr<mbgl::style::SymbolLayer>);

    ~SymbolLayer();

    // Properties

    jni::Object<jni::ObjectTag> getSymbolPlacement(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getSymbolSpacing(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getSymbolAvoidEdges(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconAllowOverlap(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconIgnorePlacement(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconOptional(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconRotationAlignment(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconSize(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconTextFit(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconTextFitPadding(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconImage(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconRotate(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconPadding(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconKeepUpright(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconOffset(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconPitchAlignment(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextPitchAlignment(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextRotationAlignment(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextField(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextFont(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextSize(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextMaxWidth(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextLineHeight(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextLetterSpacing(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextJustify(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextMaxAngle(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextRotate(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextPadding(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextKeepUpright(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextTransform(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextOffset(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextAllowOverlap(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextIgnorePlacement(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextOptional(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconOpacity(jni::JNIEnv&);
    void setIconOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getIconOpacityTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconColor(jni::JNIEnv&);
    void setIconColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getIconColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconHaloColor(jni::JNIEnv&);
    void setIconHaloColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getIconHaloColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconHaloWidth(jni::JNIEnv&);
    void setIconHaloWidthTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getIconHaloWidthTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconHaloBlur(jni::JNIEnv&);
    void setIconHaloBlurTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getIconHaloBlurTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconTranslate(jni::JNIEnv&);
    void setIconTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getIconTranslateTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getIconTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextOpacity(jni::JNIEnv&);
    void setTextOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getTextOpacityTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextColor(jni::JNIEnv&);
    void setTextColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getTextColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextHaloColor(jni::JNIEnv&);
    void setTextHaloColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getTextHaloColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextHaloWidth(jni::JNIEnv&);
    void setTextHaloWidthTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getTextHaloWidthTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextHaloBlur(jni::JNIEnv&);
    void setTextHaloBlurTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getTextHaloBlurTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextTranslate(jni::JNIEnv&);
    void setTextTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getTextTranslateTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTextTranslateAnchor(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class SymbolLayer

} // namespace android
} // namespace mbgl
