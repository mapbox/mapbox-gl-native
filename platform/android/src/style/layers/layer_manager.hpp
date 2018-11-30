#pragma once

#include <mbgl/layermanager/layer_manager.hpp>
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
class LayerManagerAndroid final : public mbgl::LayerManager {
public:
    ~LayerManagerAndroid() final;
    static LayerManagerAndroid* get() noexcept;
    
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&);
    jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>);

    void registerNative(jni::JNIEnv&);

private:
    LayerManagerAndroid();
    void addLayerType(std::unique_ptr<JavaLayerPeerFactory>);
    JavaLayerPeerFactory* getPeerFactory(const mbgl::style::LayerTypeInfo*);
    // mbgl::LayerManager overrides.
    LayerFactory* getFactory(const std::string& type) noexcept final;
    LayerFactory* getFactory(const mbgl::style::LayerTypeInfo* info) noexcept final;

    std::vector<std::unique_ptr<JavaLayerPeerFactory>> factories;
    std::map<std::string, mbgl::LayerFactory*> typeToFactory;
};

}  // namespace android
}  // namespace mbgl
