#pragma once

#include "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/custom_layer.hpp>

namespace mbgl {
    
class OpenGLStyleLayerPeerFactory : public LayerPeerFactory, public mbgl::style::CustomLayerFactory {
    // LayerPeerFactory overrides.
    style::LayerFactory* getCoreLayerFactory() final { return this; }
    virtual MGLStyleLayer* createPeer(style::Layer*) final;
};
    
}  // namespace mbgl
