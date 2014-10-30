#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/map/tile.hpp>
#include <mbgl/map/tile_data.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class Map;
class Painter;
class SourceInfo;
class StyleLayer;

class RasterTileData : public TileData {
    friend class TileParser;

public:
    RasterTileData(Tile::ID const& id, Map &map, const util::ptr<SourceInfo> &source);
    ~RasterTileData();

    virtual void parse();
    virtual void render(Painter &painter, util::ptr<StyleLayer> layer_desc, const mat4 &matrix);
    virtual bool hasData(StyleLayer const& layer_desc) const;

protected:
    StyleBucketRaster properties;
    RasterBucket bucket;
};

}

#endif
