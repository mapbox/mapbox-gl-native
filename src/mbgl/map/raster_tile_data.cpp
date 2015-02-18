#include <mbgl/map/map.hpp>
#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;


RasterTileData::RasterTileData(Tile::ID const& id_, TexturePool& texturePool, const SourceInfo& source_, FileSource& fileSource_)
    : TileData(id_, source_, fileSource_),
    bucket(texturePool, layout) {
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

bool RasterTileData::hasData(StyleLayer const& /*layer_desc*/) const {
    return bucket.hasData();
}
