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

class CustomJavaLayerPeerFactory final : public JavaLayerPeerFactory,  public mbgl::style::CustomLayerFactory {
public:
    ~CustomJavaLayerPeerFactory() override;

    // JavaLayerPeerFactory overrides.
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) final;
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) final;

    void registerNative(jni::JNIEnv&) final;

    style::LayerFactory* getLayerFactory() final { return this; }

};  // class CustomJavaLayerPeerFactory

} // namespace android
} // namespace mbgl
