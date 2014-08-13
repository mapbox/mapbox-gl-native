#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/map/tile.hpp>
#include <mbgl/map/tile_data.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

#include <memory>

namespace mbgl {

class Map;
class Painter;
class SourceInfo;
class StyleLayer;

class RasterTileData : public TileData {
    friend class TileParser;

public:
    RasterTileData(Tile::ID id, Map &map, const SourceInfo &source);
    ~RasterTileData();

    virtual void parse();
    virtual void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const mat4 &matrix);
    virtual bool hasData(std::shared_ptr<StyleLayer> layer_desc) const;

protected:
    StyleBucketRaster properties;
    RasterBucket bucket;
};

}

#endif
