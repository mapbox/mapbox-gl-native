#pragma once

#include <mbgl/util/color.hpp>

#include <set>
#include <vector>

namespace mbgl {

class Tile;
class Bucket;

namespace style {
class Layer;
class Source;
}

class RenderItem {
public:
    RenderItem(const style::Layer& layer_,
               const Tile* tile_ = nullptr,
               Bucket* bucket_ = nullptr)
        : tile(tile_), bucket(bucket_), layer(layer_) {
    }

    const Tile* const tile;
    Bucket* const bucket;
    const style::Layer& layer;
};

class RenderData {
public:
    Color backgroundColor = {{ 0, 0, 0, 0 }};
    std::set<style::Source*> sources;
    std::vector<RenderItem> order;
};

} // namespace mbgl
