#pragma once

#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/normalization.hpp>

#include <cstdint>

namespace mbgl {
namespace attributes {

// Layout attributes

MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_pos);
MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_extrude);
MBGL_DEFINE_ATTRIBUTE(uint16_t, 2, a_texture_pos);

template <std::size_t N>
struct a_data : gl::Attribute<uint8_t, N> {
    static auto name() { return "a_data"; }
};

template <std::size_t N>
struct a_offset : gl::Attribute<int16_t, N> {
    static auto name() { return "a_offset"; }
};

// Paint attributes

template <class Attr>
struct Min : Attr {
    static auto name() {
        static const std::string name = Attr::name() + std::string("_min");
        return name.c_str();
    }
};

template <class Attr>
struct Max : Attr {
    static auto name() {
        static const std::string name = Attr::name() + std::string("_max");
        return name.c_str();
    }
};

template <class Attr>
struct InterpolationUniform : gl::UniformScalar<InterpolationUniform<Attr>, float> {
    static auto name() {
        static const std::string name = Attr::name() + std::string("_t");
        return name.c_str();
    }
};

struct a_color : gl::Attribute<gl::Normalized<uint8_t>, 4> {
    static auto name() { return "a_color"; }

    static Value value(const Color& color) {
        return {{
            gl::Normalized<uint8_t>(color.r),
            gl::Normalized<uint8_t>(color.g),
            gl::Normalized<uint8_t>(color.b),
            gl::Normalized<uint8_t>(color.a)
        }};
    }
};

struct a_stroke_color : gl::Attribute<gl::Normalized<uint8_t>, 4> {
    static auto name() { return "a_stroke_color"; }

    static Value value(const Color& color) {
        return {{
            gl::Normalized<uint8_t>(color.r),
            gl::Normalized<uint8_t>(color.g),
            gl::Normalized<uint8_t>(color.b),
            gl::Normalized<uint8_t>(color.a)
        }};
    }
};

struct a_outline_color : gl::Attribute<gl::Normalized<uint8_t>, 4> {
    static auto name() { return "a_outline_color"; }

    static Value value(const Color& color) {
        return {{
            gl::Normalized<uint8_t>(color.r),
            gl::Normalized<uint8_t>(color.g),
            gl::Normalized<uint8_t>(color.b),
            gl::Normalized<uint8_t>(color.a)
        }};
    }
};

struct a_opacity : gl::Attribute<gl::Normalized<uint8_t>, 1> {
    static auto name() { return "a_opacity"; }

    static Value value(float opacity) {
        return {{ gl::Normalized<uint8_t>(opacity) }};
    }
};

struct a_stroke_opacity : gl::Attribute<gl::Normalized<uint8_t>, 1> {
    static auto name() { return "a_stroke_opacity"; }

    static Value value(float opacity) {
        return {{ gl::Normalized<uint8_t>(opacity) }};
    }
};

struct a_blur : gl::Attribute<float, 1> {
    static auto name() { return "a_blur"; }

    static Value value(float blur) {
        return {{ blur }};
    }
};

struct a_radius : gl::Attribute<float, 1> {
    static auto name() { return "a_radius"; }

    static Value value(float radius) {
        return {{ radius }};
    }
};

struct a_width : gl::Attribute<float, 1> {
    static auto name() { return "a_width"; }

    static Value value(float width) {
        return {{ width }};
    }
};

struct a_height : gl::Attribute<float, 1> {
    static auto name() { return "a_height"; }

    static Value value(float width) {
        return {{ width }};
    }
};

struct a_base : gl::Attribute<float, 1> {
    static auto name() { return "a_base"; }

    static Value value(float width) {
        return {{ width }};
    }
};

struct a_gap_width : gl::Attribute<float, 1> {
    static auto name() { return "a_gapwidth"; }

    static Value value(float width) {
        return {{ width }};
    }
};

struct a_stroke_width : gl::Attribute<float, 1> {
    static auto name() { return "a_stroke_width"; }

    static Value value(float width) {
        return {{ width }};
    }
};

template <>
struct a_offset<1> : gl::Attribute<float, 1> {
    static auto name() { return "a_offset"; }

    static Value value(float offset) {
        return {{ offset }};
    }
};

} // namespace attributes
} // namespace mbgl
