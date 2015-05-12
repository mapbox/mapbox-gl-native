#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;

RasterTileData::RasterTileData(const TileID& id_, TexturePool &texturePool,
                               const SourceInfo &source_)
    : TileData(id_, source_), bucket(texturePool, layout) {
}

RasterTileData::~RasterTileData() {
    // Cancel in most derived class destructor so that worker tasks are joined before
    // any member data goes away.
    cancel();
}

void RasterTileData::parse() {
    if (getState() != State::loaded) {
        return;
    }

    if (bucket.setImage(data)) {
        setState(State::parsed);
    } else {
        setState(State::invalid);
    }
}

Bucket* RasterTileData::getBucket(StyleLayer const&) {
    return &bucket;
}
