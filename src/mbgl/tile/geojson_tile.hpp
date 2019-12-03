#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/util/feature.hpp>

namespace mbgl {
namespace style {
class GeoJSONData;
} // namespace style

class TileParameters;

class GeoJSONTile : public GeometryTile {
public:
    GeoJSONTile(const OverscaledTileID&,
                std::string sourceID,
                const TileParameters&,
                std::shared_ptr<style::GeoJSONData>);

    void updateData(std::shared_ptr<style::GeoJSONData> data, bool needsRelayout = false);

    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;

private:
    std::shared_ptr<style::GeoJSONData> data;
    mapbox::base::WeakPtrFactory<GeoJSONTile> weakFactory{this};
};

} // namespace mbgl
