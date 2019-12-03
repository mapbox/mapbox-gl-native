// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/hillshade_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<HillshadeProgram> {
    static constexpr const char* name = "hillshade";
    static constexpr const uint8_t hash[8] = {0x8a, 0x11, 0x29, 0x18, 0x52, 0x7f, 0x3b, 0xbb};
    static constexpr const auto vertexOffset = 29125;
    static constexpr const auto fragmentOffset = 29296;
};

constexpr const char* ShaderSource<HillshadeProgram>::name;
constexpr const uint8_t ShaderSource<HillshadeProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<HillshadeProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<HillshadeProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of hillshade.vertex.glsl:
/*
uniform mat4 u_matrix;

attribute vec2 a_pos;
attribute vec2 a_texture_pos;

varying vec2 v_pos;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    v_pos = a_texture_pos / 8192.0;
}

*/

// Uncompressed source of hillshade.fragment.glsl:
/*
uniform sampler2D u_image;
varying vec2 v_pos;

uniform vec2 u_latrange;
uniform vec2 u_light;
uniform vec4 u_shadow;
uniform vec4 u_highlight;
uniform vec4 u_accent;

#define PI 3.141592653589793

void main() {
    vec4 pixel = texture2D(u_image, v_pos);

    vec2 deriv = ((pixel.rg * 2.0) - 1.0);

    // We divide the slope by a scale factor based on the cosin of the pixel's approximate latitude
    // to account for mercator projection distortion. see #4807 for details
    float scaleFactor = cos(radians((u_latrange[0] - u_latrange[1]) * (1.0 - v_pos.y) + u_latrange[1]));
    // We also multiply the slope by an arbitrary z-factor of 1.25
    float slope = atan(1.25 * length(deriv) / scaleFactor);
    float aspect = deriv.x != 0.0 ? atan(deriv.y, -deriv.x) : PI / 2.0 * (deriv.y > 0.0 ? 1.0 : -1.0);

    float intensity = u_light.x;
    // We add PI to make this property match the global light object, which adds PI/2 to the light's azimuthal
    // position property to account for 0deg corresponding to north/the top of the viewport in the style spec
    // and the original shader was written to accept (-illuminationDirection - 90) as the azimuthal.
    float azimuth = u_light.y + PI;

    // We scale the slope exponentially based on intensity, using a calculation similar to
    // the exponential interpolation function in the style spec:
    // https://github.com/mapbox/mapbox-gl-js/blob/master/src/style-spec/expression/definitions/interpolate.js#L217-L228
    // so that higher intensity values create more opaque hillshading.
    float base = 1.875 - intensity * 1.75;
    float maxValue = 0.5 * PI;
    float scaledSlope = intensity != 0.5 ? ((pow(base, slope) - 1.0) / (pow(base, maxValue) - 1.0)) * maxValue : slope;

    // The accent color is calculated with the cosine of the slope while the shade color is calculated with the sine
    // so that the accent color's rate of change eases in while the shade color's eases out.
    float accent = cos(scaledSlope);
    // We multiply both the accent and shade color by a clamped intensity value
    // so that intensities >= 0.5 do not additionally affect the color values
    // while intensity values < 0.5 make the overall color more transparent.
    vec4 accent_color = (1.0 - accent) * u_accent * clamp(intensity * 2.0, 0.0, 1.0);
    float shade = abs(mod((aspect + azimuth) / PI + 0.5, 2.0) - 1.0);
    vec4 shade_color = mix(u_shadow, u_highlight, shade) * sin(scaledSlope) * clamp(intensity * 2.0, 0.0, 1.0);
    gl_FragColor = accent_color * (1.0 - shade_color.a) + shade_color;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
