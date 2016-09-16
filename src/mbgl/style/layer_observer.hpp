#pragma once

namespace mbgl {
namespace style {

class Layer;

class LayerObserver {
public:
    virtual ~LayerObserver() = default;

    virtual void onLayerFilterChanged(Layer&) {
    }
    virtual void onLayerVisibilityChanged(Layer&) {
    }
    virtual void onLayerPaintPropertyChanged(Layer&) {
    }
    virtual void onLayerLayoutPropertyChanged(Layer&) {
    }
};

} // namespace style
} // namespace mbgl
