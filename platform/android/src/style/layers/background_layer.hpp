// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/background_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class BackgroundLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/BackgroundLayer"; };

    static jni::Class<BackgroundLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    BackgroundLayer(jni::JNIEnv&, jni::String);

    BackgroundLayer(mbgl::Map&, mbgl::style::BackgroundLayer&);

    BackgroundLayer(mbgl::Map&, std::unique_ptr<mbgl::style::BackgroundLayer>);

    ~BackgroundLayer();

    // Properties

    jni::Object<jni::ObjectTag> getBackgroundColor(jni::JNIEnv&);
    void setBackgroundColorTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getBackgroundColorTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getBackgroundPattern(jni::JNIEnv&);
    void setBackgroundPatternTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getBackgroundPatternTransition(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getBackgroundOpacity(jni::JNIEnv&);
    void setBackgroundOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Object<TransitionOptions> getBackgroundOpacityTransition(jni::JNIEnv&);
    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class BackgroundLayer

} // namespace android
} // namespace mbgl
