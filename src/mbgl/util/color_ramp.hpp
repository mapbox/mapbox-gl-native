#pragma once

#include <mbgl/util/image.hpp>
#include <mbgl/style/color_ramp_property_value.hpp>

namespace mbgl {
namespace util {

void renderColorRamp(PremultipliedImage& colorRamp, const style::ColorRampPropertyValue& expression);

} // namespace util
} // namespace mbgl
