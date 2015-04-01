#include <mbgl/style/style_layer_group.hpp>

namespace mbgl {

void StyleLayerGroup::setClasses(const std::vector<std::string> &class_names, std::chrono::steady_clock::time_point now,
                                 const PropertyTransition &defaultTransition) {
    for (const auto& layer : layers) {
        if (layer) {
            layer->setClasses(class_names, now, defaultTransition);
        }
    }
}

void StyleLayerGroup::updateProperties(float z, std::chrono::steady_clock::time_point now, ZoomHistory &zoomHistory) {
    for (const auto& layer : layers) {
        if (layer) {
            layer->updateProperties(z, now, zoomHistory);
        }
    }
}

bool StyleLayerGroup::hasTransitions() const {
    for (const auto& layer : layers) {
        if (layer) {
            if (layer->hasTransitions()) {
                return true;
            }
        }
    }
    return false;
}


}
