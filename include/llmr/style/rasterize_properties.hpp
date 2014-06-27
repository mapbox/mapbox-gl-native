#ifndef LLMR_STYLE_RASTERIZE_PROPERTIES
#define LLMR_STYLE_RASTERIZE_PROPERTIES

#include <llmr/style/function_properties.hpp>

#include <boost/optional.hpp>

namespace llmr {

struct PrerenderProperties {
    float buffer = 1.0f / 32.0f;
    uint16_t size = 256;
    uint16_t blur = 0;
};

class RasterizeProperties {
public:
    boost::optional<FunctionProperty> enabled;
    boost::optional<FunctionProperty> buffer;
    boost::optional<FunctionProperty> size;
    boost::optional<FunctionProperty> blur;
};

}

#endif
