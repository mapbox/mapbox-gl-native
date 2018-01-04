#pragma once

#include "layer.hpp"
#include <mbgl/style/layers/custom_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class CustomLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/CustomLayer"; };

    static jni::Class<CustomLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    CustomLayer(jni::JNIEnv&, jni::String, jni::jlong, jni::jlong, jni::jlong, jni::jlong, jni::jlong);

    CustomLayer(mbgl::Map&, mbgl::style::CustomLayer&);

    CustomLayer(mbgl::Map&, std::unique_ptr<mbgl::style::CustomLayer>);

    ~CustomLayer();

    void update(jni::JNIEnv&);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class CustomLayer

} // namespace android
} // namespace mbgl
