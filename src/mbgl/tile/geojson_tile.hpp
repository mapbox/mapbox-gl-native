#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/util/feature.hpp>

namespace mbgl {

class TileParameters;

class GeoJSONTile : public GeometryTile {
public:
    GeoJSONTile(const OverscaledTileID&,
                std::string sourceID,
                const TileParameters&,
                mapbox::geometry::feature_collection<int16_t>);

    void updateData(mapbox::geometry::feature_collection<int16_t>);
    
    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;
};

} // namespace mbgl
