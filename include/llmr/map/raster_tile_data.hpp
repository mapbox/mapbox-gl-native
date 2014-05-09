#ifndef LLMR_MAP_RASTER_TILE_DATA
#define LLMR_MAP_RASTER_TILE_DATA

#include <llmr/map/tile_data.hpp>

#include <llmr/renderer/raster_bucket.hpp>


namespace llmr {

class RasterTileData : public TileData {
    friend class TileParser;

public:
    RasterTileData(Tile::ID id, Map &map, const std::string url);
    ~RasterTileData();

    virtual void parse();
    virtual void render(Painter &painter, const LayerDescription& layer_desc);

protected:
    RasterBucket bucket;
};

}

#endif
