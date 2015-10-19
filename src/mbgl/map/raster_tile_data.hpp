#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class SourceInfo;
class Request;
class StyleLayer;
class TexturePool;
class WorkRequest;

class RasterTileData : public TileData {
public:
    RasterTileData(const TileID&, TexturePool&, const SourceInfo&, Worker&);
    ~RasterTileData();

    void request(float pixelRatio,
                 const std::function<void()>& callback);

    void cancel() override;

    Bucket* getBucket(StyleLayer const &layer_desc) override;

private:
    const SourceInfo& source;
    Worker& worker;
    Request* req = nullptr;

    RasterLayoutProperties layout;
    RasterBucket bucket;

    std::unique_ptr<WorkRequest> workRequest;
};

}

#endif
