#pragma once

#include <mbgl/style/layer_observer.hpp>

using namespace mbgl;
using namespace mbgl::style;

/**
 * An implementation of style::LayerObserver that forwards all methods to dynamically-settable lambas.
 */
class StubLayerObserver : public style::LayerObserver {
public:
    void onLayerFilterChanged(Layer& layer) override {
        if (layerFilterChanged) layerFilterChanged(layer);
    }

    void onLayerVisibilityChanged(Layer& layer) override {
        if (layerVisibilityChanged) layerVisibilityChanged(layer);
    }

    void onLayerPaintPropertyChanged(Layer& layer) override {
        if (layerPaintPropertyChanged) layerPaintPropertyChanged(layer);
    }

    void onLayerDataDrivenPaintPropertyChanged(Layer& layer) override {
        if (layerDataDrivenPaintPropertyChanged) layerDataDrivenPaintPropertyChanged(layer);
    }

    void onLayerLayoutPropertyChanged(Layer& layer, const char * property) override {
        if (layerLayoutPropertyChanged) layerLayoutPropertyChanged(layer, property);
    }

    std::function<void (Layer&)> layerFilterChanged;
    std::function<void (Layer&)> layerVisibilityChanged;
    std::function<void (Layer&)> layerPaintPropertyChanged;
    std::function<void (Layer&)> layerDataDrivenPaintPropertyChanged;
    std::function<void (Layer&, const char *)> layerLayoutPropertyChanged;
};
