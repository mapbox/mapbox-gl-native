#include <llmr/style/types.hpp>

#include <iostream>

namespace llmr {

StyleLayerType parseStyleLayerType(const std::string &type) {
    if (type == "fill") return StyleLayerType::Fill;
    if (type == "line") return StyleLayerType::Line;
    if (type == "icon") return StyleLayerType::Icon;
    if (type == "text") return StyleLayerType::Text;
    if (type == "raster") return StyleLayerType::Raster;
    if (type == "composite") return StyleLayerType::Composite;
    if (type == "background") return StyleLayerType::Background;
    return StyleLayerType::Unknown;
}

const char *stringifyStyleLayerType(StyleLayerType type) {
    switch (type) {
        case StyleLayerType::Fill: return "fill";
        case StyleLayerType::Line: return "line";
        case StyleLayerType::Icon: return "icon";
        case StyleLayerType::Text: return "text";
        case StyleLayerType::Raster: return "raster";
        case StyleLayerType::Composite: return "composite";
        case StyleLayerType::Background: return "background";
        default: return "unknown";
    }
}

std::ostream& operator<<(std::ostream& os, StyleLayerType type) {
    return os << stringifyStyleLayerType(type);
}

}
