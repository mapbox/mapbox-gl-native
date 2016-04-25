#pragma once

namespace mbgl {

class Layer;
class Tile;
class Bucket;

struct RenderItem {
    inline RenderItem(const Layer& layer_,
                      const Tile* tile_ = nullptr,
                      Bucket* bucket_ = nullptr)
        : tile(tile_), bucket(bucket_), layer(layer_) {
    }

    const Tile* const tile;
    Bucket* const bucket;
    const Layer& layer;
};

} // namespace mbgl
