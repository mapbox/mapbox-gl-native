// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/circle_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class CircleLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/CircleLayer"; };

    static jni::Class<CircleLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    CircleLayer(jni::JNIEnv&, jni::String, jni::String);

    CircleLayer(mbgl::Map&, mbgl::style::CircleLayer&);

    CircleLayer(mbgl::Map&, std::unique_ptr<mbgl::style::CircleLayer>);

    ~CircleLayer();

    // Properties

    jni::Object<jni::ObjectTag> getCircleRadius(jni::JNIEnv&);
    void setCircleRadiusTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getCircleRadiusTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleColor(jni::JNIEnv&);
    void setCircleColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getCircleColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleBlur(jni::JNIEnv&);
    void setCircleBlurTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getCircleBlurTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleOpacity(jni::JNIEnv&);
    void setCircleOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getCircleOpacityTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleTranslate(jni::JNIEnv&);
    void setCircleTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getCircleTranslateTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCirclePitchScale(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCirclePitchAlignment(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleStrokeWidth(jni::JNIEnv&);
    void setCircleStrokeWidthTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getCircleStrokeWidthTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleStrokeColor(jni::JNIEnv&);
    void setCircleStrokeColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getCircleStrokeColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleStrokeOpacity(jni::JNIEnv&);
    void setCircleStrokeOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getCircleStrokeOpacityTransition(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class CircleLayer

} // namespace android
} // namespace mbgl
