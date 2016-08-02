// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#pragma once

#include "layer.hpp"
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

    ~BackgroundLayer();

    jni::jobject* createJavaPeer(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
