// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/line_pattern_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<LinePatternProgram> {
    static constexpr const char* name = "line_pattern";
    static constexpr const uint8_t hash[8] = {0x38, 0x9c, 0x3d, 0xde, 0xb4, 0xe0, 0xd1, 0x61};
    static constexpr const auto vertexOffset = 37858;
    static constexpr const auto fragmentOffset = 41252;
};

constexpr const char* ShaderSource<LinePatternProgram>::name;
constexpr const uint8_t ShaderSource<LinePatternProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<LinePatternProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<LinePatternProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of line_pattern.vertex.glsl:
/*
// floor(127 / 2) == 63.0
// the maximum allowed miter limit is 2.0 at the moment. the extrude normal is
// stored in a byte (-128..127). we scale regular normals up to length 63, but
// there are also "special" normals that have a bigger length (of up to 126 in
// this case).
// #define scale 63.0
#define scale 0.015873016

// We scale the distance before adding it to the buffers so that we can store
// long distances for long segments. Use this value to unscale the distance.
#define LINE_DISTANCE_SCALE 2.0

attribute vec2 a_pos_normal;
attribute vec4 a_data;

uniform mat4 u_matrix;
uniform vec2 u_units_to_pixels;
uniform mediump float u_ratio;
uniform lowp float u_device_pixel_ratio;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_linesofar;
varying float v_gamma_scale;


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


#ifndef HAS_UNIFORM_u_offset
uniform lowp float u_offset_t;
attribute lowp vec2 a_offset;
#else
uniform lowp float u_offset;
#endif


#ifndef HAS_UNIFORM_u_gapwidth
uniform lowp float u_gapwidth_t;
attribute mediump vec2 a_gapwidth;
#else
uniform mediump float u_gapwidth;
#endif


#ifndef HAS_UNIFORM_u_width
uniform lowp float u_width_t;
attribute mediump vec2 a_width;
#else
uniform mediump float u_width;
#endif


#ifndef HAS_UNIFORM_u_pattern_from
uniform lowp float u_pattern_from_t;
attribute lowp vec4 a_pattern_from;
varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif


#ifndef HAS_UNIFORM_u_pattern_to
uniform lowp float u_pattern_to_t;
attribute lowp vec4 a_pattern_to;
varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
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

    
#ifndef HAS_UNIFORM_u_offset
    lowp float offset = unpack_mix_vec2(a_offset, u_offset_t);
#else
    lowp float offset = u_offset;
#endif

    
#ifndef HAS_UNIFORM_u_gapwidth
    mediump float gapwidth = unpack_mix_vec2(a_gapwidth, u_gapwidth_t);
#else
    mediump float gapwidth = u_gapwidth;
#endif

    
#ifndef HAS_UNIFORM_u_width
    mediump float width = unpack_mix_vec2(a_width, u_width_t);
#else
    mediump float width = u_width;
#endif

    
#ifndef HAS_UNIFORM_u_pattern_from
    pattern_from = a_pattern_from;
#else
    mediump vec4 pattern_from = u_pattern_from;
#endif

    
#ifndef HAS_UNIFORM_u_pattern_to
    pattern_to = a_pattern_to;
#else
    mediump vec4 pattern_to = u_pattern_to;
#endif


    // the distance over which the line edge fades out.
    // Retina devices need a smaller distance to avoid aliasing.
    float ANTIALIASING = 1.0 / u_device_pixel_ratio / 2.0;

    vec2 a_extrude = a_data.xy - 128.0;
    float a_direction = mod(a_data.z, 4.0) - 1.0;
    float a_linesofar = (floor(a_data.z / 4.0) + a_data.w * 64.0) * LINE_DISTANCE_SCALE;
    // float tileRatio = u_scale.y;
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

    v_linesofar = a_linesofar;
    v_width2 = vec2(outset, inset);
}

*/

// Uncompressed source of line_pattern.fragment.glsl:
/*
uniform lowp float u_device_pixel_ratio;
uniform vec2 u_texsize;
uniform float u_fade;
uniform mediump vec4 u_scale;

uniform sampler2D u_image;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_linesofar;
varying float v_gamma_scale;


#ifndef HAS_UNIFORM_u_pattern_from
varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif


#ifndef HAS_UNIFORM_u_pattern_to
varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif


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
    
#ifdef HAS_UNIFORM_u_pattern_from
    mediump vec4 pattern_from = u_pattern_from;
#endif

    
#ifdef HAS_UNIFORM_u_pattern_to
    mediump vec4 pattern_to = u_pattern_to;
#endif


    
#ifdef HAS_UNIFORM_u_blur
    lowp float blur = u_blur;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif


    vec2 pattern_tl_a = pattern_from.xy;
    vec2 pattern_br_a = pattern_from.zw;
    vec2 pattern_tl_b = pattern_to.xy;
    vec2 pattern_br_b = pattern_to.zw;

    float pixelRatio = u_scale.x;
    float tileZoomRatio = u_scale.y;
    float fromScale = u_scale.z;
    float toScale = u_scale.w;

    vec2 display_size_a = vec2((pattern_br_a.x - pattern_tl_a.x) / pixelRatio, (pattern_br_a.y - pattern_tl_a.y) / pixelRatio);
    vec2 display_size_b = vec2((pattern_br_b.x - pattern_tl_b.x) / pixelRatio, (pattern_br_b.y - pattern_tl_b.y) / pixelRatio);

    vec2 pattern_size_a = vec2(display_size_a.x * fromScale / tileZoomRatio, display_size_a.y);
    vec2 pattern_size_b = vec2(display_size_b.x * toScale / tileZoomRatio, display_size_b.y);

    // Calculate the distance of the pixel from the line in pixels.
    float dist = length(v_normal) * v_width2.s;

    // Calculate the antialiasing fade factor. This is either when fading in
    // the line in case of an offset line (v_width2.t) or when fading out
    // (v_width2.s)
    float blur2 = (blur + 1.0 / u_device_pixel_ratio) * v_gamma_scale;
    float alpha = clamp(min(dist - (v_width2.t - blur2), v_width2.s - dist) / blur2, 0.0, 1.0);

    float x_a = mod(v_linesofar / pattern_size_a.x, 1.0);
    float x_b = mod(v_linesofar / pattern_size_b.x, 1.0);

    // v_normal.y is 0 at the midpoint of the line, -1 at the lower edge, 1 at the upper edge
    // we clamp the line width outset to be between 0 and half the pattern height plus padding (2.0)
    // to ensure we don't sample outside the designated symbol on the sprite sheet.
    // 0.5 is added to shift the component to be bounded between 0 and 1 for interpolation of
    // the texture coordinate
    float y_a = 0.5 + (v_normal.y * clamp(v_width2.s, 0.0, (pattern_size_a.y + 2.0) / 2.0) / pattern_size_a.y);
    float y_b = 0.5 + (v_normal.y * clamp(v_width2.s, 0.0, (pattern_size_b.y + 2.0) / 2.0) / pattern_size_b.y);
    vec2 pos_a = mix(pattern_tl_a / u_texsize, pattern_br_a / u_texsize, vec2(x_a, y_a));
    vec2 pos_b = mix(pattern_tl_b / u_texsize, pattern_br_b / u_texsize, vec2(x_b, y_b));

    vec4 color = mix(texture2D(u_image, pos_a), texture2D(u_image, pos_b), u_fade);

    gl_FragColor = color * alpha * opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
