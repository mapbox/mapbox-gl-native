#pragma once

#include <mbgl/tile/tile_data.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class RasterTileSource;
class AsyncRequest;

namespace gl { class TexturePool; }

namespace style {
class Layer;
}

class RasterTileData : public TileData {
public:
    RasterTileData(const OverscaledTileID&,
                   std::unique_ptr<RasterTileSource>,
                   gl::TexturePool&,
                   Worker&,
                   const std::function<void(std::exception_ptr)>& callback);
    ~RasterTileData();

    void cancel() override;
    Bucket* getBucket(const style::Layer&) override;

private:
    gl::TexturePool& texturePool;
    Worker& worker;

    std::unique_ptr<AsyncRequest> tileRequest;
    std::unique_ptr<AsyncRequest> workRequest;

    // Contains the Bucket object for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unique_ptr<Bucket> bucket;
};

} // namespace mbgl
