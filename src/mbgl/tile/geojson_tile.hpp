#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/util/feature.hpp>

namespace mbgl {

namespace style {
class UpdateParameters;
} // namespace style

class GeoJSONTile : public GeometryTile {
public:
    GeoJSONTile(const OverscaledTileID&,
                std::string sourceID,
                const style::UpdateParameters&,
                mapbox::geometry::feature_collection<int16_t>);

    void updateData(mapbox::geometry::feature_collection<int16_t>);

    void setNecessity(Necessity) final;
    
    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;
};

} // namespace mbgl
