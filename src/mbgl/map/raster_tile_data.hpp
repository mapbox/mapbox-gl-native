#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/style/style_layout.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class SourceInfo;
class Request;
class StyleLayer;
class TexturePool;
class WorkRequest;

class RasterTileData : public TileData {
public:
    RasterTileData(const TileID&, TexturePool&, const SourceInfo&);
    ~RasterTileData();

    void request(Worker&,
                 float pixelRatio,
                 const std::function<void()>& callback) override;

    bool reparse(Worker&, std::function<void ()> callback) override;

    void cancel() override;

    Bucket* getBucket(StyleLayer const &layer_desc) override;

private:
    const SourceInfo& source;

    Request *req = nullptr;
    std::string data;

    StyleLayoutRaster layout;
    RasterBucket bucket;

    std::unique_ptr<WorkRequest> workRequest;
};

}

#endif
