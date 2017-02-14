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
               std::vector<std::reference_wrapper<RenderTile>> tiles_ = {})
        : layer(layer_), tiles(std::move(tiles_)) {
    }

    const style::Layer& layer;
    std::vector<std::reference_wrapper<RenderTile>> tiles;
};

class RenderData {
public:
    Color backgroundColor;
    std::unordered_set<style::Source*> sources;
    std::vector<RenderItem> order;
};

} // namespace mbgl
