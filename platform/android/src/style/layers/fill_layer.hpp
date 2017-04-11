// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/fill_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class FillLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/FillLayer"; };

    static jni::Class<FillLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    FillLayer(jni::JNIEnv&, jni::String, jni::String);

    FillLayer(mbgl::Map&, mbgl::style::FillLayer&);

    FillLayer(mbgl::Map&, std::unique_ptr<mbgl::style::FillLayer>);

    ~FillLayer();

    // Properties

    jni::Object<jni::ObjectTag> getFillAntialias(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillOpacity(jni::JNIEnv&);
    void setFillOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillOpacityTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillColor(jni::JNIEnv&);
    void setFillColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillOutlineColor(jni::JNIEnv&);
    void setFillOutlineColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillOutlineColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillTranslate(jni::JNIEnv&);
    void setFillTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillTranslateTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillPattern(jni::JNIEnv&);
    void setFillPatternTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillPatternTransition(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class FillLayer

} // namespace android
} // namespace mbgl
