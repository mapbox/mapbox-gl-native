#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/actor/mailbox.hpp>

namespace mbgl {

class TileParameters;

namespace style {
class CustomTileLoader;
} // namespace style

class CustomGeometryTile: public GeometryTile {
public:
    CustomGeometryTile(const OverscaledTileID&,
               std::string sourceID,
               const TileParameters&,
               const style::CustomGeometrySource::TileOptions,
               ActorRef<style::CustomTileLoader> loader);
    ~CustomGeometryTile() override;

    void setTileData(const GeoJSON& data);
    void invalidateTileData();

    void setNecessity(TileNecessity) final;

    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;

private:
    bool stale = true;
    TileNecessity necessity;
    const style::CustomGeometrySource::TileOptions options;
    ActorRef<style::CustomTileLoader> loader;
    std::shared_ptr<Mailbox> mailbox;
    ActorRef<CustomGeometryTile> actorRef;
};

} // namespace mbgl
