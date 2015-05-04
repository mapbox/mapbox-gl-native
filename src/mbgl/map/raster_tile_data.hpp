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
    friend class TileParser;

public:
    RasterTileData(const TileID&, TexturePool&, const SourceInfo&);
    ~RasterTileData();

    void parse() override;
    Bucket* getBucket(StyleLayer const &layer_desc) override;

protected:
    StyleLayoutRaster layout;
    RasterBucket bucket;
};

}

#endif
