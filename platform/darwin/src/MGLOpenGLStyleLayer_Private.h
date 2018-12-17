#pragma once

#include "MGLStyleLayer_Private.h"

#include <mbgl/layermanager/custom_layer_factory.hpp>

namespace mbgl {
    
class OpenGLStyleLayerPeerFactory : public LayerPeerFactory, public mbgl::CustomLayerFactory {
    // LayerPeerFactory overrides.
    LayerFactory* getCoreLayerFactory() final { return this; }
    virtual MGLStyleLayer* createPeer(style::Layer*) final;
};
    
}  // namespace mbgl
