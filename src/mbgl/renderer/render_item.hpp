#pragma once

#include <mbgl/util/color.hpp>

#include <unordered_set>
#include <vector>

namespace mbgl {

class RenderLayer;
class RenderSource;
class RenderTile;
class Bucket;

namespace style {
} // namespace style

class RenderItem {
public:
    RenderItem(RenderLayer& layer_,
               RenderSource* renderSource_)
        : layer(layer_), source(renderSource_) {
    }

    RenderLayer& layer;
    RenderSource* source;
};

class RenderData {
public:
    Color backgroundColor;
    std::unordered_set<RenderSource*> sources;
    std::vector<RenderItem> order;
};

} // namespace mbgl
