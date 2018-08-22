#pragma once

#include "layer.hpp"
#include <mbgl/style/layers/custom_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class CustomLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/CustomLayer"; };

    static void registerNative(jni::JNIEnv&);

    CustomLayer(jni::JNIEnv&, const jni::String&, jni::jlong);
    CustomLayer(mbgl::Map&, mbgl::style::CustomLayer&);
    CustomLayer(mbgl::Map&, std::unique_ptr<mbgl::style::CustomLayer>);
    ~CustomLayer();

    void update(jni::JNIEnv&);

    jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv&);

}; // class CustomLayer

} // namespace android
} // namespace mbgl
