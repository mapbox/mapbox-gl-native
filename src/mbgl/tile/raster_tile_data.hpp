#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/tile/tile_data.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class FileRequest;
class StyleLayer;
class WorkRequest;
namespace gl { class TexturePool; }

class RasterTileData : public TileData {
public:
    RasterTileData(const TileID&,
                   float pixelRatio,
                   const std::string& urlTemplate,
                   gl::TexturePool&,
                   Worker&,
                   const std::function<void(std::exception_ptr)>& callback);
    ~RasterTileData();

    void cancel() override;
    Bucket* getBucket(StyleLayer const &layer_desc) override;

private:
    gl::TexturePool& texturePool;
    Worker& worker;
    std::unique_ptr<FileRequest> req;
    std::unique_ptr<Bucket> bucket;
    std::unique_ptr<WorkRequest> workRequest;
};

} // namespace mbgl

#endif
