#pragma once

#include <mbgl/gl/attribute.hpp>

#include <cstdint>

namespace mbgl {
namespace attributes {

// Attributes common to several shaders.

MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_pos);
MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_offset);
MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_extrude);
MBGL_DEFINE_ATTRIBUTE(uint16_t, 2, a_texture_pos);

template <std::size_t N>
struct a_data : gl::Attribute<a_data<N>, uint8_t, N> {
    static constexpr auto name = "a_data";
};

} // namespace attributes
} // namespace mbgl
