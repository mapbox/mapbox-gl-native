#ifndef LLMR_STYLE_STYLE_LAYER_GROUP
#define LLMR_STYLE_STYLE_LAYER_GROUP

#include <llmr/style/style_layer.hpp>

#include <vector>

namespace llmr {

class StyleLayerGroup {
public:
    void setClasses(const std::vector<std::string> &class_names, timestamp now,
                    const PropertyTransition &defaultTransition);

public:
    std::vector<std::shared_ptr<StyleLayer>> layers;
};

}

#endif
