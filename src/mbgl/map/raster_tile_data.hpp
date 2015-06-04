#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/style/style_layout.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class Painter;
class SourceInfo;
class StyleLayer;
class TexturePool;

class RasterTileData : public TileData {
public:
    RasterTileData(const TileID&, TexturePool&, const SourceInfo&);
    ~RasterTileData();

    void request(Worker&,
                 float pixelRatio,
                 const std::function<void()>& callback) override;

    bool reparse(Worker&, std::function<void ()> callback) override;

    void parse();
    Bucket* getBucket(StyleLayer const &layer_desc) override;

protected:
    StyleLayoutRaster layout;
    RasterBucket bucket;
};

}

#endif
