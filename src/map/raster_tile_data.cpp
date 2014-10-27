#include <mbgl/map/map.hpp>
#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;


RasterTileData::RasterTileData(Tile::ID id_, Map &map_, const util::ptr<SourceInfo> &source_)
    : TileData(id_, map_, source_),
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

void RasterTileData::render(Painter &painter, util::ptr<StyleLayer> layer_desc, const mat4 &matrix) {
    bucket.render(painter, layer_desc, id, matrix);
}

bool RasterTileData::hasData(util::ptr<StyleLayer> /*layer_desc*/) const {
    return bucket.hasData();
}
