#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_loader.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class AsyncRequest;
class Tileset;

namespace style {
class Layer;
class UpdateParameters;
} // namespace style

class RasterTile : public Tile {
public:
    RasterTile(const OverscaledTileID&,
                   const style::UpdateParameters&,
                   const Tileset&);
    ~RasterTile() final;

    void setNecessity(Necessity) final;

    void setError(std::exception_ptr err);

    void setData(std::shared_ptr<const std::string> data,
                 optional<Timestamp> modified_,
                 optional<Timestamp> expires_);

    void cancel() override;
    Bucket* getBucket(const style::Layer&) override;

private:
    Worker& worker;

    TileLoader<RasterTile> loader;
    std::unique_ptr<AsyncRequest> workRequest;

    // Contains the Bucket object for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unique_ptr<Bucket> bucket;
};

} // namespace mbgl
