#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_loader.hpp>
#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/actor/actor.hpp>

namespace mbgl {

class Tileset;
class UpdateParameters;

namespace style {
class Layer;
} // namespace style

class RasterTile : public Tile {
public:
    RasterTile(const OverscaledTileID&,
                   const UpdateParameters&,
                   const Tileset&);
    ~RasterTile() final;

    void setNecessity(Necessity) final;

    void setError(std::exception_ptr);
    void setData(std::shared_ptr<const std::string> data,
                 optional<Timestamp> modified_,
                 optional<Timestamp> expires_);

    void cancel() override;
    Bucket* getBucket(const RenderLayer&) const override;

    void onParsed(std::unique_ptr<Bucket> result);
    void onError(std::exception_ptr);

private:
    TileLoader<RasterTile> loader;

    std::shared_ptr<Mailbox> mailbox;
    Actor<RasterTileWorker> worker;

    // Contains the Bucket object for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unique_ptr<Bucket> bucket;
};

} // namespace mbgl
