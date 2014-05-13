#include <llmr/map/map.hpp>
#include <llmr/map/raster_tile_data.hpp>
#include <llmr/style/layer_description.hpp>

using namespace llmr;


RasterTileData::RasterTileData(Tile::ID id, Map &map, const std::string url)
    : TileData(id, map, url),
    bucket(map.getTexturepool()) {
}

RasterTileData::~RasterTileData() {
}

void RasterTileData::parse() {
    if (state != State::loaded) {
        return;
    }

    if (bucket.setImage(data)) {
        state = State::parsed;
    } else {
        state = State::invalid;
    }
}

void RasterTileData::render(Painter &painter, const LayerDescription& layer_desc) {
    bucket.render(painter, layer_desc.name, id);
}

bool RasterTileData::hasData(const LayerDescription& layer_desc) const {
    return bucket.hasData();
}
