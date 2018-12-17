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
    /**
     * @brief Enables a layer type for both JSON style and runtime API.
     */
    void addLayerType(std::unique_ptr<JavaLayerPeerFactory>); 
    /**
     * @brief Enables a layer type for JSON style only.
     *
     * We might not want to expose runtime API for some layer types
     * in order to save binary size - JNI glue code for these layer types
     * won't be added to the binary. 
     */
    void addLayerTypeCoreOnly(std::unique_ptr<mbgl::LayerFactory>);

    void registerCoreFactory(mbgl::LayerFactory*);
    JavaLayerPeerFactory* getPeerFactory(const mbgl::style::LayerTypeInfo*);
    // mbgl::LayerManager overrides.
    LayerFactory* getFactory(const std::string& type) noexcept final;
    LayerFactory* getFactory(const mbgl::style::LayerTypeInfo* info) noexcept final;

    std::vector<std::unique_ptr<JavaLayerPeerFactory>> peerFactories;
    std::vector<std::unique_ptr<mbgl::LayerFactory>> coreFactories;
    std::map<std::string, mbgl::LayerFactory*> typeToFactory;
};

}  // namespace android
}  // namespace mbgl
