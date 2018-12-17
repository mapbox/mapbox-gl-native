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
    void addLayerType(std::unique_ptr<LayerPeerFactory>);
    LayerPeerFactory* getPeerFactory(const style::LayerTypeInfo* typeInfo);
    // mbgl::LayerManager overrides.
    LayerFactory* getFactory(const std::string& type) noexcept final;
    LayerFactory* getFactory(const mbgl::style::LayerTypeInfo* info) noexcept final;

    std::vector<std::unique_ptr<LayerPeerFactory>> factories;
    std::map<std::string, LayerFactory*> typeToFactory;
};
    
}  // namespace mbgl
