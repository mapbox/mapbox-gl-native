#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_loader.hpp>
#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/actor/actor.hpp>

namespace mbgl {

class Tileset;
class TileParameters;
class RasterBucket;

namespace style {
class Layer;
} // namespace style

class RasterTile : public Tile {
public:
    RasterTile(const OverscaledTileID&,
                   const TileParameters&,
                   const Tileset&);
    ~RasterTile() override;

    void setNecessity(TileNecessity) final;

    void setError(std::exception_ptr);
    void setMetadata(optional<Timestamp> modified, optional<Timestamp> expires);
    void setData(std::shared_ptr<const std::string> data);

    void upload(gl::Context&) override;
    Bucket* getBucket(const style::Layer::Impl&) const override;

    void setMask(TileMask&&) override;

    void onParsed(std::unique_ptr<RasterBucket> result, uint64_t correlationID);
    void onError(std::exception_ptr, uint64_t correlationID);

private:
    TileLoader<RasterTile> loader;

    std::shared_ptr<Mailbox> mailbox;
    Actor<RasterTileWorker> worker;

    uint64_t correlationID = 0;

    // Contains the Bucket object for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unique_ptr<RasterBucket> bucket;
};

} // namespace mbgl
