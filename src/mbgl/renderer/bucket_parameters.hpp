#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>

namespace mbgl {

class BucketParameters {
public:
    const OverscaledTileID tileID;
    const MapMode mode;
    const float pixelRatio;
};

} // namespace mbgl
