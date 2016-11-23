// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
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

    ~LineLayer();

    // Property getters
    jni::Object<jni::ObjectTag> getLineCap(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineJoin(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineMiterLimit(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineRoundLimit(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineOpacity(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineTranslate(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineWidth(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineGapWidth(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineOffset(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineBlur(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLineDasharray(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getLinePattern(jni::JNIEnv&);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class LineLayer

} // namespace android
} // namespace mbgl
