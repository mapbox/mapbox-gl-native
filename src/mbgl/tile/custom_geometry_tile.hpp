#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/style/custom_tile_loader.hpp>

namespace mbgl {

class TileParameters;

class CustomGeometryTile: public GeometryTile {
public:
    CustomGeometryTile(const OverscaledTileID&,
               std::string sourceID,
               const TileParameters&,
               const style::CustomGeometrySource::TileOptions,
               ActorRef<style::CustomTileLoader> loader);
    ~CustomGeometryTile() override;
    void setTileData(const GeoJSON& data);

    void setNecessity(TileNecessity) final;

    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;

private:
    TileNecessity necessity;
    const style::CustomGeometrySource::TileOptions options;
    ActorRef<style::CustomTileLoader> loader;
    Actor<style::SetTileDataFunction> actor;
};

} // namespace mbgl
