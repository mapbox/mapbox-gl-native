// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#pragma once

#include "layer.hpp"
#include <mbgl/style/layers/symbol_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class SymbolLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/SymbolLayer"; };

    static jni::Class<SymbolLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    SymbolLayer(jni::JNIEnv&, jni::String, jni::String);

    SymbolLayer(mbgl::Map&, mbgl::style::SymbolLayer&);

    ~SymbolLayer();

    jni::jobject* createJavaPeer(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
