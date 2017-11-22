#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_loader.hpp>
#include <mbgl/tile/raster_dem_tile_worker.hpp>
#include <mbgl/actor/actor.hpp>

namespace mbgl {

class Tileset;
class TileParameters;
class HillshadeBucket;

namespace style {
class Layer;
} // namespace style

class RasterDEMTile : public Tile {
public:
    RasterDEMTile(const OverscaledTileID&,
                   const TileParameters&,
                   const Tileset&);
    ~RasterDEMTile();

    void setNecessity(TileNecessity) final;

    void setError(std::exception_ptr);
    void setMetadata(optional<Timestamp> modified, optional<Timestamp> expires);
    void setData(std::shared_ptr<const std::string> data);

    void cancel() override;

    void upload(gl::Context&) override;
    Bucket* getBucket(const style::Layer::Impl&) const override;
    
    std::unordered_map<OverscaledTileID, bool> neighboringTiles;
    
    void setMask(TileMask&&) override;

    void onParsed(std::unique_ptr<HillshadeBucket> result, uint64_t correlationID);
    void onError(std::exception_ptr, uint64_t correlationID);

private:
    TileLoader<RasterDEMTile> loader;

    std::shared_ptr<Mailbox> mailbox;
    Actor<RasterDEMTileWorker> worker;
    
    uint64_t correlationID = 0;

    // Contains the Bucket object for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unique_ptr<HillshadeBucket> bucket;

};

} // namespace mbgl
