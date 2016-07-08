// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

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

    jni::jobject* createJavaPeer(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
