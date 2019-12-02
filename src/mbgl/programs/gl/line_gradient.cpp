// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/line_gradient_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<LineGradientProgram> {
    static constexpr const char* name = "line_gradient";
    static constexpr const uint8_t hash[8] = {0x3f, 0xba, 0xc6, 0x33, 0xcd, 0x86, 0xa2, 0xe8};
    static constexpr const auto vertexOffset = 34236;
    static constexpr const auto fragmentOffset = 37028;
};

constexpr const char* ShaderSource<LineGradientProgram>::name;
constexpr const uint8_t ShaderSource<LineGradientProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<LineGradientProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<LineGradientProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of line_gradient.vertex.glsl:
/*

// the attribute conveying progress along a line is scaled to [0, 2^15)
#define MAX_LINE_DISTANCE 32767.0

// floor(127 / 2) == 63.0
// the maximum allowed miter limit is 2.0 at the moment. the extrude normal is
// stored in a byte (-128..127). we scale regular normals up to length 63, but
// there are also "special" normals that have a bigger length (of up to 126 in
// this case).
// #define scale 63.0
#define scale 0.015873016

attribute vec2 a_pos_normal;
attribute vec4 a_data;

uniform mat4 u_matrix;
uniform mediump float u_ratio;
uniform lowp float u_device_pixel_ratio;
uniform vec2 u_units_to_pixels;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_gamma_scale;
varying highp float v_lineprogress;


#ifndef HAS_UNIFORM_u_blur
uniform lowp float u_blur_t;
attribute lowp vec2 a_blur;
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


#ifndef HAS_UNIFORM_u_gapwidth
uniform lowp float u_gapwidth_t;
attribute mediump vec2 a_gapwidth;
#else
uniform mediump float u_gapwidth;
#endif


#ifndef HAS_UNIFORM_u_offset
uniform lowp float u_offset_t;
attribute lowp vec2 a_offset;
#else
uniform lowp float u_offset;
#endif


#ifndef HAS_UNIFORM_u_width
uniform lowp float u_width_t;
attribute mediump vec2 a_width;
#else
uniform mediump float u_width;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_blur
    blur = unpack_mix_vec2(a_blur, u_blur_t);
#else
    lowp float blur = u_blur;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, u_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif

    
#ifndef HAS_UNIFORM_u_gapwidth
    mediump float gapwidth = unpack_mix_vec2(a_gapwidth, u_gapwidth_t);
#else
    mediump float gapwidth = u_gapwidth;
#endif

    
#ifndef HAS_UNIFORM_u_offset
    lowp float offset = unpack_mix_vec2(a_offset, u_offset_t);
#else
    lowp float offset = u_offset;
#endif

    
#ifndef HAS_UNIFORM_u_width
    mediump float width = unpack_mix_vec2(a_width, u_width_t);
#else
    mediump float width = u_width;
#endif


    // the distance over which the line edge fades out.
    // Retina devices need a smaller distance to avoid aliasing.
    float ANTIALIASING = 1.0 / u_device_pixel_ratio / 2.0;

    vec2 a_extrude = a_data.xy - 128.0;
    float a_direction = mod(a_data.z, 4.0) - 1.0;

    v_lineprogress = (floor(a_data.z / 4.0) + a_data.w * 64.0) * 2.0 / MAX_LINE_DISTANCE;

    vec2 pos = floor(a_pos_normal * 0.5);

    // x is 1 if it's a round cap, 0 otherwise
    // y is 1 if the normal points up, and -1 if it points down
    // We store these in the least significant bit of a_pos_normal
    mediump vec2 normal = a_pos_normal - 2.0 * pos;
    normal.y = normal.y * 2.0 - 1.0;
    v_normal = normal;

    // these transformations used to be applied in the JS and native code bases.
    // moved them into the shader for clarity and simplicity.
    gapwidth = gapwidth / 2.0;
    float halfwidth = width / 2.0;
    offset = -1.0 * offset;

    float inset = gapwidth + (gapwidth > 0.0 ? ANTIALIASING : 0.0);
    float outset = gapwidth + halfwidth * (gapwidth > 0.0 ? 2.0 : 1.0) + (halfwidth == 0.0 ? 0.0 : ANTIALIASING);

    // Scale the extrusion vector down to a normal and then up by the line width
    // of this vertex.
    mediump vec2 dist = outset * a_extrude * scale;

    // Calculate the offset when drawing a line that is to the side of the actual line.
    // We do this by creating a vector that points towards the extrude, but rotate
    // it when we're drawing round end points (a_direction = -1 or 1) since their
    // extrude vector points in another direction.
    mediump float u = 0.5 * a_direction;
    mediump float t = 1.0 - abs(u);
    mediump vec2 offset2 = offset * a_extrude * scale * normal.y * mat2(t, -u, u, t);

    vec4 projected_extrude = u_matrix * vec4(dist / u_ratio, 0.0, 0.0);
    gl_Position = u_matrix * vec4(pos + offset2 / u_ratio, 0.0, 1.0) + projected_extrude;

    // calculate how much the perspective view squishes or stretches the extrude
    float extrude_length_without_perspective = length(dist);
    float extrude_length_with_perspective = length(projected_extrude.xy / gl_Position.w * u_units_to_pixels);
    v_gamma_scale = extrude_length_without_perspective / extrude_length_with_perspective;

    v_width2 = vec2(outset, inset);
}

*/

// Uncompressed source of line_gradient.fragment.glsl:
/*
uniform lowp float u_device_pixel_ratio;
uniform sampler2D u_image;

varying vec2 v_width2;
varying vec2 v_normal;
varying float v_gamma_scale;
varying highp float v_lineprogress;


#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif


#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_blur
    lowp float blur = u_blur;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif


    // Calculate the distance of the pixel from the line in pixels.
    float dist = length(v_normal) * v_width2.s;

    // Calculate the antialiasing fade factor. This is either when fading in
    // the line in case of an offset line (v_width2.t) or when fading out
    // (v_width2.s)
    float blur2 = (blur + 1.0 / u_device_pixel_ratio) * v_gamma_scale;
    float alpha = clamp(min(dist - (v_width2.t - blur2), v_width2.s - dist) / blur2, 0.0, 1.0);

    // For gradient lines, v_lineprogress is the ratio along the entire line,
    // scaled to [0, 2^15), and the gradient ramp is stored in a texture.
    vec4 color = texture2D(u_image, vec2(v_lineprogress, 0.5));

    gl_FragColor = color * (alpha * opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
