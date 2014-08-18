#include <mbgl/map/map.hpp>
#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;


RasterTileData::RasterTileData(Tile::ID id, Map &map, const SourceInfo &source)
    : TileData(id, map, source),
    bucket(map.getTexturepool(), properties) {
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

void RasterTileData::render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const mat4 &matrix) {
    bucket.render(painter, layer_desc, id, matrix);
}

bool RasterTileData::hasData(std::shared_ptr<StyleLayer> /*layer_desc*/) const {
    return bucket.hasData();
}
