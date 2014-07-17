#ifndef MBGL_STYLE_STYLE_LAYER_GROUP
#define MBGL_STYLE_STYLE_LAYER_GROUP

#include <mbgl/style/style_layer.hpp>

#include <vector>

namespace mbgl {

class StyleLayerGroup {
public:
    void setClasses(const std::vector<std::string> &class_names, timestamp now,
                    const PropertyTransition &defaultTransition);
    void updateProperties(float z, timestamp t);

    bool hasTransitions() const;
public:
    std::vector<std::shared_ptr<StyleLayer>> layers;
};

}

#endif
