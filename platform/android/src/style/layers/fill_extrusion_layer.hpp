// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
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

    ~FillExtrusionLayer();

    // Property getters
    jni::Object<jni::ObjectTag> getFillExtrusionOpacity(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionTranslate(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionTranslateAnchor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionPattern(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionHeight(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getFillExtrusionBase(jni::JNIEnv&);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class FillExtrusionLayer

} // namespace android
} // namespace mbgl
