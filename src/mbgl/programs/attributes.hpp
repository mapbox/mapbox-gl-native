#pragma once

#include <mbgl/gfx/attribute.hpp>

namespace mbgl {
namespace attributes {

// Layout attributes

MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_pos);
MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_extrude);
MBGL_DEFINE_ATTRIBUTE(int16_t, 4, a_pos_offset);
MBGL_DEFINE_ATTRIBUTE(int16_t, 4, a_pos_normal);
MBGL_DEFINE_ATTRIBUTE(float, 3, a_projected_pos);
MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_label_pos);
MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_anchor_pos);
MBGL_DEFINE_ATTRIBUTE(uint16_t, 2, a_texture_pos);
MBGL_DEFINE_ATTRIBUTE(int16_t,  4, a_normal_ed);
MBGL_DEFINE_ATTRIBUTE(uint8_t, 1, a_fade_opacity);
MBGL_DEFINE_ATTRIBUTE(uint8_t, 2, a_placed);
MBGL_DEFINE_ATTRIBUTE(uint16_t, 3, a_size);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_offset);
MBGL_DEFINE_ATTRIBUTE(float, 2, a_shift);

template <typename T, std::size_t N>
struct a_data {
    using Type = gfx::AttributeType<T, N>;
    static auto name() { return "a_data"; }
};

// Paint attributes

MBGL_DEFINE_ATTRIBUTE(float, 2, a_color);
MBGL_DEFINE_ATTRIBUTE(float, 2, a_fill_color);
MBGL_DEFINE_ATTRIBUTE(float, 2, a_halo_color);
MBGL_DEFINE_ATTRIBUTE(float, 2, a_stroke_color);
MBGL_DEFINE_ATTRIBUTE(float, 2, a_outline_color);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_opacity);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_stroke_opacity);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_blur);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_radius);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_width);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_floorwidth);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_height);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_base);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_gapwidth);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_stroke_width);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_halo_width);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_halo_blur);
MBGL_DEFINE_ATTRIBUTE(float, 1, a_weight);
MBGL_DEFINE_ATTRIBUTE(uint16_t, 4, a_pattern_to);
MBGL_DEFINE_ATTRIBUTE(uint16_t, 4, a_pattern_from);

} // namespace attributes

using PositionOnlyLayoutAttributes = TypeList<attributes::a_pos>;

} // namespace mbgl
