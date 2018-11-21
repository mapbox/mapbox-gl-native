#pragma once

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layer.hpp>

#include <map>
#include <string>
#include <vector>

namespace mbgl {

class LayerManagerDarwin : public style::LayerManager {
public:
    static LayerManagerDarwin* get() noexcept;
    ~LayerManagerDarwin();

    MGLStyleLayer* createPeer(style::Layer*);

private:
    LayerManagerDarwin();
    void addLayerType(std::unique_ptr<LayerPeerFactory>);
    // LayerManager overrides.
    std::unique_ptr<style::Layer> createLayer(const std::string& type, const std::string& id, const style::conversion::Convertible& value, style::conversion::Error& error) noexcept final;
    
    std::vector<std::unique_ptr<LayerPeerFactory>> factories;
    std::map<std::string, style::LayerFactory*> typeToFactory;
};
    
}  // namespace mbgl
