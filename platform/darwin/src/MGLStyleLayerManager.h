#pragma once

#import "MGLStyleLayer_Private.h"

#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/style/layer.hpp>

#include <map>
#include <string>
#include <vector>

namespace mbgl {

class LayerManagerDarwin : public LayerManager {
public:
    static LayerManagerDarwin* get() noexcept;
    ~LayerManagerDarwin();

    MGLStyleLayer* createPeer(style::Layer*);

private:
    LayerManagerDarwin();
    /**
     * Enables a layer type for both JSON style and runtime API.
     */
    void addLayerType(std::unique_ptr<LayerPeerFactory>);
    /**
     * Enables a layer type for JSON style only.
     *
     * We might not want to expose runtime API for some layer types
     * in order to save binary size (the corresponding SDK layer wrappers
     * should be excluded from the project build).
     */
    void addLayerTypeCoreOnly(std::unique_ptr<mbgl::LayerFactory>);

    void registerCoreFactory(LayerFactory*);
    LayerPeerFactory* getPeerFactory(const style::LayerTypeInfo* typeInfo);
    // mbgl::LayerManager overrides.
    LayerFactory* getFactory(const std::string& type) noexcept final;
    LayerFactory* getFactory(const mbgl::style::LayerTypeInfo* info) noexcept final;

    std::vector<std::unique_ptr<LayerPeerFactory>> peerFactories;
    std::vector<std::unique_ptr<LayerFactory>> coreFactories;
    std::map<std::string, LayerFactory*> typeToFactory;
};
    
}  // namespace mbgl
