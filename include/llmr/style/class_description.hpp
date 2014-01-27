#ifndef LLMR_STYLE_CLASS_DESCRIPTION
#define LLMR_STYLE_CLASS_DESCRIPTION

#include <string>
#include <vector>
#include <map>
#include "properties.hpp"

namespace llmr {


class WidthDescription {
public:
    std::string scaling;
    std::vector<float> value;
};


class LayerStyleDescription {
public:
    Color color = {{ 0, 0, 0, 0 }};
    bool antialias = false;
    WidthDescription width;
};

typedef std::map<std::string, LayerStyleDescription> ClassDescription;

std::ostream& operator<<(std::ostream&, const ClassDescription& layer);

}

#endif
