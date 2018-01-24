// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class HillshadeLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/HillshadeLayer"; };

    static jni::Class<HillshadeLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    HillshadeLayer(jni::JNIEnv&, jni::String, jni::String);

    HillshadeLayer(mbgl::Map&, mbgl::style::HillshadeLayer&);

    HillshadeLayer(mbgl::Map&, std::unique_ptr<mbgl::style::HillshadeLayer>);

    ~HillshadeLayer();

    // Properties

    jni::Object<jni::ObjectTag> getHillshadeIlluminationDirection(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getHillshadeIlluminationAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getHillshadeExaggeration(jni::JNIEnv&);
    void setHillshadeExaggerationTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getHillshadeExaggerationTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getHillshadeShadowColor(jni::JNIEnv&);
    void setHillshadeShadowColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getHillshadeShadowColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getHillshadeHighlightColor(jni::JNIEnv&);
    void setHillshadeHighlightColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getHillshadeHighlightColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getHillshadeAccentColor(jni::JNIEnv&);
    void setHillshadeAccentColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getHillshadeAccentColorTransition(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class HillshadeLayer

} // namespace android
} // namespace mbgl
