// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
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

    ~CircleLayer();

    // Property getters
    jni::Object<jni::ObjectTag> getCircleRadius(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleBlur(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleOpacity(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleTranslate(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCirclePitchScale(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleStrokeWidth(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleStrokeColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getCircleStrokeOpacity(jni::JNIEnv&);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class CircleLayer

} // namespace android
} // namespace mbgl
