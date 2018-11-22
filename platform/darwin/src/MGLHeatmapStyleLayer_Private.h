// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
#pragma once

#include "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/heatmap_layer.hpp>

namespace mbgl {

class HeatmapStyleLayerPeerFactory : public LayerPeerFactory, public mbgl::HeatmapLayerFactory {
    // LayerPeerFactory overrides.
    LayerFactory* getCoreLayerFactory() final { return this; }
    virtual MGLStyleLayer* createPeer(style::Layer*) final;
};

}  // namespace mbgl
