#ifndef LLMR_STYLE_RASTERIZE_PROPERTIES
#define LLMR_STYLE_RASTERIZE_PROPERTIES

#include <llmr/style/function_properties.hpp>

namespace llmr {

class RasterizeProperties {
public:
    Function<bool> enabled = ConstantFunction<bool>(false);
    Function<float> buffer = ConstantFunction<float>(1.0f / 32.0f);
    Function<float> size = ConstantFunction<float>(256);
    Function<float> blur = ConstantFunction<float>(0);
};

// The calculated properties for a layer in a tile.
class RasterizedProperties {
public:
    bool enabled = false;
    float buffer = 1.0f / 32.0f;
    uint16_t size = 256;
    uint8_t blur = 0;
};

}

#endif
