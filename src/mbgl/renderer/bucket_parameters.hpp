#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>

namespace mbgl {
namespace style {
    struct LayerTypeInfo;
} // namespace style

class BucketParameters {
public:
    const OverscaledTileID tileID;
    const MapMode mode;
    const float pixelRatio;
    const style::LayerTypeInfo* layerType;
};

} // namespace mbgl
