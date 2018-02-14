#include <mbgl/util/color_ramp.hpp>

#include <cmath>

namespace mbgl {
namespace util {

void renderColorRamp(PremultipliedImage& colorRamp, const style::ColorRampPropertyValue& expression) {
    const auto length = colorRamp.bytes() / 4;

    for (uint32_t i = 0; i < length; i++) {
        uint32_t j = i * 4;
        const auto color = expression.evaluate(static_cast<double>(i) / (length - 1));
        colorRamp.data[j + 0] = std::floor(color.r * 255);
        colorRamp.data[j + 1] = std::floor(color.g * 255);
        colorRamp.data[j + 2] = std::floor(color.b * 255);
        colorRamp.data[j + 3] = std::floor(color.a * 255);
    }
}

} // namespace util
} // namespace mbgl
