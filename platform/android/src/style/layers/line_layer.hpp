// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#pragma once

#include "layer.hpp"
#include <mbgl/style/layers/line_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class LineLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/LineLayer"; };

    static jni::Class<LineLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    LineLayer(jni::JNIEnv&, jni::String, jni::String);

    LineLayer(mbgl::Map&, mbgl::style::LineLayer&);

    ~LineLayer();

    jni::jobject* createJavaPeer(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
