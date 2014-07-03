#ifndef LLMR_STYLE_RASTERIZE_PROPERTIES
#define LLMR_STYLE_RASTERIZE_PROPERTIES

#include <llmr/style/function_properties.hpp>

namespace llmr {

// The calculated properties for a layer in a tile.
class RasterizedProperties {
public:
    float buffer = 1.0f / 32.0f;
    uint16_t size = 256;
    uint8_t blur = 0;
};

class RasterizeProperties {
public:
    Function<bool> enabled = ConstantFunction<bool>(false);
    Function<float> buffer = ConstantFunction<float>(1.0f / 32.0f);
    Function<float> size = ConstantFunction<float>(256);
    Function<float> blur = ConstantFunction<float>(0);

    inline bool isEnabled(const int8_t z) const {
        return util::apply_visitor(FunctionEvaluator<bool>(z), enabled);
    }

    inline RasterizedProperties get(const int8_t z) const {
        RasterizedProperties properties;
        properties.buffer = util::apply_visitor(FunctionEvaluator<float>(z), buffer);
        properties.size = util::apply_visitor(FunctionEvaluator<float>(z), size);
        properties.blur = util::apply_visitor(FunctionEvaluator<float>(z), blur);
        return properties;
    }
};

}

#endif
