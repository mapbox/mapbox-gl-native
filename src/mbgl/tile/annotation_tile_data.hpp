#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

namespace style {
class UpdateParameters;
}

class AnnotationTileData : public GeometryTileData {
public:
    AnnotationTileData(const OverscaledTileID&,
                       std::string sourceID,
                       const style::UpdateParameters&);
};

} // namespace mbgl
