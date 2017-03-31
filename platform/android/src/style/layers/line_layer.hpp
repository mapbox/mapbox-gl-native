// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/line_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class LineLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/LineLayer"; };

    static jni::Class<LineLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    LineLayer(jni::JNIEnv&, jni::String, jni::String);

    LineLayer(mbgl::Map&, mbgl::style::LineLayer&);

    LineLayer(mbgl::Map&, std::unique_ptr<mbgl::style::LineLayer>);

    ~LineLayer();

    // Properties

    jni::Object<jni::ObjectTag> getLineCap(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineJoin(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineMiterLimit(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineRoundLimit(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineOpacity(jni::JNIEnv&);
    void setLineOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLineOpacityTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineColor(jni::JNIEnv&);
    void setLineColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLineColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineTranslate(jni::JNIEnv&);
    void setLineTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLineTranslateTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineWidth(jni::JNIEnv&);
    void setLineWidthTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLineWidthTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineGapWidth(jni::JNIEnv&);
    void setLineGapWidthTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLineGapWidthTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineOffset(jni::JNIEnv&);
    void setLineOffsetTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLineOffsetTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineBlur(jni::JNIEnv&);
    void setLineBlurTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLineBlurTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineDasharray(jni::JNIEnv&);
    void setLineDasharrayTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLineDasharrayTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLinePattern(jni::JNIEnv&);
    void setLinePatternTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getLinePatternTransition(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class LineLayer

} // namespace android
} // namespace mbgl
