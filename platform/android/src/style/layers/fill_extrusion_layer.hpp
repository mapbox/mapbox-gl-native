// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class FillExtrusionLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/FillExtrusionLayer"; };

    static jni::Class<FillExtrusionLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    FillExtrusionLayer(jni::JNIEnv&, jni::String, jni::String);

    FillExtrusionLayer(mbgl::Map&, mbgl::style::FillExtrusionLayer&);

    FillExtrusionLayer(mbgl::Map&, std::unique_ptr<mbgl::style::FillExtrusionLayer>);

    ~FillExtrusionLayer();

    // Properties

    jni::Object<jni::ObjectTag> getFillExtrusionOpacity(jni::JNIEnv&);
    void setFillExtrusionOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillExtrusionOpacityTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionColor(jni::JNIEnv&);
    void setFillExtrusionColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillExtrusionColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionTranslate(jni::JNIEnv&);
    void setFillExtrusionTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillExtrusionTranslateTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionPattern(jni::JNIEnv&);
    void setFillExtrusionPatternTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillExtrusionPatternTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionHeight(jni::JNIEnv&);
    void setFillExtrusionHeightTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillExtrusionHeightTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionBase(jni::JNIEnv&);
    void setFillExtrusionBaseTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getFillExtrusionBaseTransition(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class FillExtrusionLayer

} // namespace android
} // namespace mbgl
