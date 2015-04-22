#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;

RasterTileData::RasterTileData(const TileID& id_, TexturePool &texturePool,
                               const SourceInfo &source_)
    : TileData(id_, source_), bucket(texturePool, layout) {
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

Bucket* RasterTileData::getBucket(StyleLayer const&) {
    return &bucket;
}
