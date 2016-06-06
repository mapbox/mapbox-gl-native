#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

class AnnotationTileData : public GeometryTileData {
public:
    using GeometryTileData::GeometryTileData;
};

} // namespace mbgl
