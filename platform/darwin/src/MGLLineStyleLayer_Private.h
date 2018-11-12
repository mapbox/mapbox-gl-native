// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
#pragma once

#include "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/line_layer.hpp>

namespace mbgl {

class LineStyleLayerPeerFactory : public LayerPeerFactory, public mbgl::style::LineLayerFactory {
    // LayerPeerFactory overrides.
    style::LayerFactory* getCoreLayerFactory() final { return this; }
    virtual MGLStyleLayer* createPeer(style::Layer*) final;
};

}  // namespace mbgl
