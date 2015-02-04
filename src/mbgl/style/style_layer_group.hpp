#ifndef MBGL_STYLE_STYLE_LAYER_GROUP
#define MBGL_STYLE_STYLE_LAYER_GROUP

#include <mbgl/style/style_layer.hpp>

#include <vector>
#include <chrono>

namespace mbgl {

class StyleLayerGroup {
public:
    void setClasses(const std::vector<std::string> &class_names, std::chrono::steady_clock::time_point now,
                    const PropertyTransition &defaultTransition);
    void updateProperties(float z, std::chrono::steady_clock::time_point now, ZoomHistory &zoomHistory);

    bool hasTransitions() const;
public:
    std::vector<util::ptr<StyleLayer>> layers;
};

}

#endif
