// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
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

    ~FillLayer();

    // Property getters
    jni::Object<jni::ObjectTag> getFillAntialias(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillOpacity(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillOutlineColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillTranslate(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillPattern(jni::JNIEnv&);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class FillLayer

} // namespace android
} // namespace mbgl
