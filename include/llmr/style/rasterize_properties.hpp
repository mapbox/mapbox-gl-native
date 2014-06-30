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
    boost::optional<Function<bool>> enabled;
    boost::optional<Function<float>> buffer;
    boost::optional<Function<float>> size;
    boost::optional<Function<float>> blur;
};

}

#endif
