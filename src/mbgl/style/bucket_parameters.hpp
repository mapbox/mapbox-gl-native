#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>

namespace mbgl {
namespace style {

class BucketParameters {
public:
    const OverscaledTileID tileID;
    const MapMode mode;
};

} // namespace style
} // namespace mbgl
