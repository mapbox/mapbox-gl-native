#pragma once

#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>

#include <cstdint>

namespace mbgl {
namespace attributes {

/*
 * Pack a pair of values, interpreted as uint8's, into a single float.
 * Used to conserve vertex attributes. Values are unpacked in the vertex
 * shader using the `unpack_float()` function, defined in _prelude.vertex.glsl.
 */
template <typename T>
inline uint16_t packUint8Pair(T a, T b) {
    return static_cast<uint16_t>(a) * 256 + static_cast<uint16_t>(b);
}

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

template <typename T, std::size_t N>
struct a_data {
    static auto name() { return "a_data"; }
    using Type = gl::AttributeType<T, N>;
};

struct a_size {
    static auto name() { return "a_size"; }
    using Type = gl::AttributeType<uint16_t, 3>;
};

template <std::size_t N>
struct a_offset {
    static auto name() { return "a_offset"; }
    using Type = gl::AttributeType<int16_t, N>;
};

// Paint attributes

struct a_color {
    static auto name() { return "a_color"; }
    using Type = gl::AttributeType<float, 2>;
};

struct a_fill_color {
    static auto name() { return "a_fill_color"; }
    using Type = gl::AttributeType<float, 2>;
};

struct a_halo_color {
    static auto name() { return "a_halo_color"; }
    using Type = gl::AttributeType<float, 2>;
};

struct a_stroke_color {
    static auto name() { return "a_stroke_color"; }
    using Type = gl::AttributeType<float, 2>;
};

struct a_outline_color {
    static auto name() { return "a_outline_color"; }
    using Type = gl::AttributeType<float, 2>;
};

struct a_opacity {
    static auto name() { return "a_opacity"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_stroke_opacity {
    static auto name() { return "a_stroke_opacity"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_blur {
    static auto name() { return "a_blur"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_radius {
    static auto name() { return "a_radius"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_width {
    static auto name() { return "a_width"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_floorwidth {
    static auto name() { return "a_floorwidth"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_height {
    static auto name() { return "a_height"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_base {
    static auto name() { return "a_base"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_gapwidth {
    static auto name() { return "a_gapwidth"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_stroke_width {
    static auto name() { return "a_stroke_width"; }
    using Type = gl::AttributeType<float, 1>;
};

template <>
struct a_offset<1> {
    static auto name() { return "a_offset"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_halo_width {
    static auto name() { return "a_halo_width"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_halo_blur {
    static auto name() { return "a_halo_blur"; }
    using Type = gl::AttributeType<float, 1>;
};

struct a_weight {
    static auto name() { return "a_weight"; }
    using Type = gl::AttributeType<float, 1>;
};

} // namespace attributes

struct PositionOnlyLayoutAttributes : gl::Attributes<
    attributes::a_pos>
{};

} // namespace mbgl
