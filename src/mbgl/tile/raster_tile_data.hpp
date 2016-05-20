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
                   gl::TexturePool&,
                   Worker&,
                   const std::function<void(std::exception_ptr)>& callback);
    ~RasterTileData();

    void setData(std::exception_ptr err,
                 std::shared_ptr<const std::string> data,
                 optional<Timestamp> modified_,
                 optional<Timestamp> expires_);

    void cancel() override;
    Bucket* getBucket(const style::Layer&) override;

private:
    gl::TexturePool& texturePool;
    Worker& worker;

    std::unique_ptr<AsyncRequest> workRequest;

    // Contains the Bucket object for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unique_ptr<Bucket> bucket;

    const std::function<void(std::exception_ptr)> callback;
};

} // namespace mbgl
