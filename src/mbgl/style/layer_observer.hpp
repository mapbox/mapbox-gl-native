#pragma once

namespace mbgl {
namespace style {

class Layer;

class LayerObserver {
public:
    virtual ~LayerObserver() = default;

    virtual void onLayerChanged(Layer&) {}
};

} // namespace style
} // namespace mbgl
