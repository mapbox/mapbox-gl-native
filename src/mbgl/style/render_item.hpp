#pragma once

namespace mbgl {

class Tile;
class Bucket;

namespace style {
class Layer;
}

struct RenderItem {
    inline RenderItem(const style::Layer& layer_,
                      const Tile* tile_ = nullptr,
                      Bucket* bucket_ = nullptr)
        : tile(tile_), bucket(bucket_), layer(layer_) {
    }

    const Tile* const tile;
    Bucket* const bucket;
    const style::Layer& layer;
};

} // namespace mbgl
