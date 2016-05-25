#pragma once

namespace mbgl {

class StyleLayer;
class Tile;
class Bucket;

struct RenderItem {
    inline RenderItem(const StyleLayer& layer_,
                      const Tile* tile_ = nullptr,
                      Bucket* bucket_ = nullptr)
        : tile(tile_), bucket(bucket_), layer(layer_) {
    }

    const Tile* const tile;
    Bucket* const bucket;
    const StyleLayer& layer;
};

} // namespace mbgl
