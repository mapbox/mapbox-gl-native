#pragma once

#include <mbgl/util/color.hpp>

#include <unordered_set>
#include <vector>

namespace mbgl {

class RenderTile;
class Bucket;

namespace style {
class Layer;
class Source;
} // namespace style

class RenderItem {
public:
    RenderItem(const style::Layer& layer_,
               const RenderTile* tile_ = nullptr,
               Bucket* bucket_ = nullptr)
        : tile(tile_), bucket(bucket_), layer(layer_) {
    }

    const RenderTile* const tile;
    Bucket* const bucket;
    const style::Layer& layer;
};

class RenderData {
public:
    Color backgroundColor;
    std::unordered_set<style::Source*> sources;
    std::vector<RenderItem> order;
};

} // namespace mbgl
