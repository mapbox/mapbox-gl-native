#ifndef MBGL_UTIL_INTERACTIVE_FEATURES
#define MBGL_UTIL_INTERACTIVE_FEATURES

#include <map>
#include <string>

namespace mbgl {

struct FeatureDescription {
    inline explicit FeatureDescription(std::string layer_, std::string source_, std::map<std::string, std::string>properties_)
    : layer(layer_), source(source_), properties(properties_) {}

    std::string layer;
    std::string source;
    std::map<std::string, std::string> properties;
};

}

#endif
