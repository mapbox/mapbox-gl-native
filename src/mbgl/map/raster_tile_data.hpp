#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class SourceInfo;
class FileRequest;
class StyleLayer;
class TexturePool;
class WorkRequest;

class RasterTileData : public TileData {
public:
    RasterTileData(const TileID&, TexturePool&, const SourceInfo&, Worker&);
    ~RasterTileData();

    using Callback = std::function<void()>;

    void request(float pixelRatio,
                 const Callback& callback);

    void cancel() override;

    Bucket* getBucket(StyleLayer const &layer_desc) override;

private:
    TexturePool& texturePool;
    const SourceInfo& source;
    Worker& worker;
    std::unique_ptr<FileRequest> req;

    std::unique_ptr<Bucket> bucket;

    std::unique_ptr<WorkRequest> workRequest;
};

}

#endif
