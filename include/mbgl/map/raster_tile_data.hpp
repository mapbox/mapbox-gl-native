#ifndef MBGL_MAP_RASTER_TILE_DATA
#define MBGL_MAP_RASTER_TILE_DATA

#include <mbgl/map/tile_data.hpp>

#include <mbgl/renderer/raster_bucket.hpp>


namespace mbgl {

class RasterTileData : public TileData {
    friend class TileParser;

public:
    RasterTileData(Tile::ID id, Map &map, const SourceInfo &source);
    ~RasterTileData();

    virtual void parse();
    virtual void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc);
    virtual bool hasData(std::shared_ptr<StyleLayer> layer_desc) const;

protected:
    RasterBucket bucket;
};

}

#endif
