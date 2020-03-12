#pragma once

#include <jni/jni.hpp>
#include <mbgl/gl/custom_layer.hpp>
#include <mbgl/gl/custom_layer_factory.hpp>
#include "layer.hpp"

namespace mbgl {
namespace android {

class CustomLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/CustomLayer"; };

    static void registerNative(jni::JNIEnv&);

    CustomLayer(jni::JNIEnv&, const jni::String&, jni::jlong);
    CustomLayer(mbgl::style::CustomLayer&);
    CustomLayer(std::unique_ptr<mbgl::style::CustomLayer>);
    ~CustomLayer();

    jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv&);
}; // class CustomLayer

class CustomJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::CustomLayerFactory {
public:
    ~CustomJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    LayerFactory* getLayerFactory() final { return this; }

};  // class CustomJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
