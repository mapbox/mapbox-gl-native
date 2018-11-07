#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/style/layer.hpp>

#include "layer.hpp"

#include <jni/jni.hpp>

#include <memory>
#include <vector>

namespace mbgl {
namespace android {

/**
 * @brief A singleton class forwarding calls to the corresponding  \c JavaLayerPeerFactory instance.
 */
class LayerManagerAndroid : public mbgl::style::LayerManager {
public:
    ~LayerManagerAndroid() override;
    static LayerManagerAndroid* get();
    
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&);
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>);

    void registerNative(jni::JNIEnv&);

private:
    LayerManagerAndroid();
    JavaLayerPeerFactory* getPeerFactory(mbgl::style::Layer*);
    // mbgl:style::LayerManager overrides.
    std::unique_ptr<style::Layer> createLayer(const std::string& type, const std::string& id, const style::conversion::Convertible& value, style::conversion::Error& error) final;

    std::vector<std::unique_ptr<JavaLayerPeerFactory>> factories;
};

}
}
