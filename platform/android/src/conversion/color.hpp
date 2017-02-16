#pragma once

#include "conversion.hpp"

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<mbgl::Color, int> {
    Result<mbgl::Color> operator()(jni::JNIEnv&, const int& color) const {
        float r = (color >> 16) & 0xFF;
        float g = (color >> 8) & 0xFF;
        float b = (color) & 0xFF;
        float a = (color >> 24) & 0xFF;
        return { mbgl::Color( r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f ) };
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
