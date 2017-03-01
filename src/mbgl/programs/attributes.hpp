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
MBGL_DEFINE_ATTRIBUTE(int16_t, 4, a_pos_offset);
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

/*
   ZoomInterpolatedAttribute<Attr> is a 'compound' attribute, representing two values of the
   the base attribute Attr.  These two values are provided to the shader to allow interpolation
   between zoom levels, without the need to repopulate vertex buffers each frame as the map is
   being zoomed.
*/
template <class Attr>
struct ZoomInterpolatedAttribute : gl::Attribute<typename Attr::ValueType, Attr::Dimensions * 2> {
    using Value = typename gl::Attribute<typename Attr::ValueType, Attr::Dimensions * 2>::Value;

    static auto name() {
        return Attr::name();
    }
    
    template <class InputType>
    static Value value(const InputType& min, const InputType& max){
        auto minValue = Attr::value(min);
        auto maxValue = Attr::value(max);
        Value result = {{}};
        for (size_t i = 0; i < Attr::Dimensions; i++) {
            result[i] = minValue[i];
            result[Attr::Dimensions+i] = maxValue[i];
        }
        return result;
    }
};

template <class Attr>
struct InterpolationUniform : gl::UniformScalar<InterpolationUniform<Attr>, float> {
    static auto name() {
        static const std::string name = Attr::name() + std::string("_t");
        return name.c_str();
    }
};

/*
    Encode a four-component color value into a pair of floats.  Since csscolorparser
    uses 8-bit precision for each color component, for each float we use the upper 8
    bits for one component (e.g. (color.r * 255) * 256), and the lower 8 for another.
    
    Also note:
     - Colors come in as floats 0..1, so we scale by 255.
     - Casting the scaled values to ints is important: without doing this, e.g., the
       fractional part of the `r` component would corrupt the lower-8 bits of the encoded
       value, which must be reserved for the `g` component.
*/
static std::array<float, 2> encodeColor (const Color& color) {
    const auto v1 = static_cast<uint16_t>( static_cast<uint16_t>(color.r*255)*256 + color.g*255);
    const auto v2 = static_cast<uint16_t>( static_cast<uint16_t>(color.b*255)*256 + color.a*255);
    return {{ static_cast<float>(v1), static_cast<float>(v2) }};
}

struct a_color : gl::Attribute<float, 2> {
    static auto name() { return "a_color"; }

    static Value value(const Color& color) {
        return encodeColor(color);
    }
};

// used in the symbol sdf shader
struct a_fill_color : gl::Attribute<float, 2> {
    static auto name() { return "a_fill_color"; }
    
    static Value value(const Color& color) {
        return encodeColor(color);
    }
};

// used in the symbol sdf shader
struct a_halo_color : gl::Attribute<float, 2> {
    static auto name() { return "a_halo_color"; }
    
 static Value value(const Color& color) {
        return encodeColor(color);
    }
};

struct a_stroke_color : gl::Attribute<float, 2> {
    static auto name() { return "a_stroke_color"; }

    static Value value(const Color& color) {
        return encodeColor(color);
    }
};

struct a_outline_color : gl::Attribute<float, 2> {
    static auto name() { return "a_outline_color"; }

    static Value value(const Color& color) {
        return encodeColor(color);
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

struct a_halo_width : gl::Attribute<float, 1> {
    static auto name() { return "a_halo_width"; }
    
    static Value value(float width) {
        return {{ width }};
    }
};

struct a_halo_blur : gl::Attribute<float, 1> {
    static auto name() { return "a_halo_blur"; }
    
    static Value value(float blur) {
        return {{ blur }};
    }
};



} // namespace attributes
} // namespace mbgl
