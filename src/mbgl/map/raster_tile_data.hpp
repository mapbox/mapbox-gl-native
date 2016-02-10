#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class FileRequest;
class StyleLayer;
class TexturePool;
class WorkRequest;

class RasterTileData : public TileData {
public:
    RasterTileData(const TileID&,
                   float pixelRatio,
                   const std::string& urlTemplate,
                   TexturePool&,
                   Worker&,
                   const std::function<void(std::exception_ptr)>& callback);
    ~RasterTileData();

    void cancel() override;
    Bucket* getBucket(StyleLayer const &layer_desc) override;

private:
    TexturePool& texturePool;
    Worker& worker;
    std::unique_ptr<FileRequest> req;
    std::unique_ptr<Bucket> bucket;
    std::unique_ptr<WorkRequest> workRequest;
};

} // namespace mbgl

#endif
