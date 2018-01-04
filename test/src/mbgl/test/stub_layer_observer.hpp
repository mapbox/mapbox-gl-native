#pragma once

#include <mbgl/style/layer_observer.hpp>

using namespace mbgl;
using namespace mbgl::style;

/**
 * An implementation of style::LayerObserver that forwards all methods to dynamically-settable lambas.
 */
class StubLayerObserver : public style::LayerObserver {
public:
    void onLayerChanged(Layer& layer) override {
        if (layerChanged) layerChanged(layer);
    }

    std::function<void (Layer&)> layerChanged;
};
