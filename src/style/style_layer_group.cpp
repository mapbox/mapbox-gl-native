#include <llmr/style/style_layer_group.hpp>

namespace llmr {

void StyleLayerGroup::setClasses(const std::vector<std::string> &class_names, timestamp now,
                                 const PropertyTransition &defaultTransition) {
    for (const std::shared_ptr<StyleLayer> &layer : layers) {
        if (layer) {
            layer->setClasses(class_names, now, defaultTransition);
        }
    }
}

}
