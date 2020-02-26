// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/symbol_text_and_icon_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<SymbolTextAndIconProgram> {
    static constexpr const char* name = "symbol_text_and_icon";
    static constexpr const uint8_t hash[8] = {0x7e, 0xbe, 0x72, 0x43, 0x55, 0x8e, 0xb5, 0xeb};
    static constexpr const auto vertexOffset = 59277;
    static constexpr const auto fragmentOffset = 63333;
};

constexpr const char* ShaderSource<SymbolTextAndIconProgram>::name;
constexpr const uint8_t ShaderSource<SymbolTextAndIconProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<SymbolTextAndIconProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<SymbolTextAndIconProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of symbol_text_and_icon.vertex.glsl:
/*
const float PI = 3.141592653589793;

attribute vec4 a_pos_offset;
attribute vec4 a_data;
attribute vec3 a_projected_pos;
attribute float a_fade_opacity;

// contents of a_size vary based on the type of property value
// used for {text,icon}-size.
// For constants, a_size is disabled.
// For source functions, we bind only one value per vertex: the value of {text,icon}-size evaluated for the current feature.
// For composite functions:
// [ text-size(lowerZoomStop, feature),
//   text-size(upperZoomStop, feature) ]
uniform bool u_is_size_zoom_constant;
uniform bool u_is_size_feature_constant;
uniform highp float u_size_t; // used to interpolate between zoom stops when size is a composite function
uniform highp float u_size; // used when size is both zoom and feature constant
uniform mat4 u_matrix;
uniform mat4 u_label_plane_matrix;
uniform mat4 u_coord_matrix;
uniform bool u_is_text;
uniform bool u_pitch_with_map;
uniform highp float u_pitch;
uniform bool u_rotate_symbol;
uniform highp float u_aspect_ratio;
uniform highp float u_camera_to_center_distance;
uniform float u_fade_change;
uniform vec2 u_texsize;
uniform vec2 u_texsize_icon;

varying vec4 v_data0;
varying vec4 v_data1;


#ifndef HAS_UNIFORM_u_fill_color
uniform lowp float u_fill_color_t;
attribute highp vec4 a_fill_color;
varying highp vec4 fill_color;
#else
uniform highp vec4 u_fill_color;
#endif


#ifndef HAS_UNIFORM_u_halo_color
uniform lowp float u_halo_color_t;
attribute highp vec4 a_halo_color;
varying highp vec4 halo_color;
#else
uniform highp vec4 u_halo_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


#ifndef HAS_UNIFORM_u_halo_width
uniform lowp float u_halo_width_t;
attribute lowp vec2 a_halo_width;
varying lowp float halo_width;
#else
uniform lowp float u_halo_width;
#endif


#ifndef HAS_UNIFORM_u_halo_blur
uniform lowp float u_halo_blur_t;
attribute lowp vec2 a_halo_blur;
varying lowp float halo_blur;
#else
uniform lowp float u_halo_blur;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_fill_color
    fill_color = unpack_mix_color(a_fill_color, u_fill_color_t);
#else
    highp vec4 fill_color = u_fill_color;
#endif

    
#ifndef HAS_UNIFORM_u_halo_color
    halo_color = unpack_mix_color(a_halo_color, u_halo_color_t);
#else
    highp vec4 halo_color = u_halo_color;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, u_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif

    
#ifndef HAS_UNIFORM_u_halo_width
    halo_width = unpack_mix_vec2(a_halo_width, u_halo_width_t);
#else
    lowp float halo_width = u_halo_width;
#endif

    
#ifndef HAS_UNIFORM_u_halo_blur
    halo_blur = unpack_mix_vec2(a_halo_blur, u_halo_blur_t);
#else
    lowp float halo_blur = u_halo_blur;
#endif


    vec2 a_pos = a_pos_offset.xy;
    vec2 a_offset = a_pos_offset.zw;

    vec2 a_tex = a_data.xy;
    vec2 a_size = a_data.zw;

    float a_size_min = floor(a_size[0] * 0.5);
    float is_sdf = a_size[0] - 2.0 * a_size_min;

    highp float segment_angle = -a_projected_pos[2];
    float size;

    if (!u_is_size_zoom_constant && !u_is_size_feature_constant) {
        size = mix(a_size_min, a_size[1], u_size_t) / 128.0;
    } else if (u_is_size_zoom_constant && !u_is_size_feature_constant) {
        size = a_size_min / 128.0;
    } else {
        size = u_size;
    }

    vec4 projectedPoint = u_matrix * vec4(a_pos, 0, 1);
    highp float camera_to_anchor_distance = projectedPoint.w;
    // If the label is pitched with the map, layout is done in pitched space,
    // which makes labels in the distance smaller relative to viewport space.
    // We counteract part of that effect by multiplying by the perspective ratio.
    // If the label isn't pitched with the map, we do layout in viewport space,
    // which makes labels in the distance larger relative to the features around
    // them. We counteract part of that effect by dividing by the perspective ratio.
    highp float distance_ratio = u_pitch_with_map ?
        camera_to_anchor_distance / u_camera_to_center_distance :
        u_camera_to_center_distance / camera_to_anchor_distance;
    highp float perspective_ratio = clamp(
        0.5 + 0.5 * distance_ratio,
        0.0, // Prevents oversized near-field symbols in pitched/overzoomed tiles
        4.0);

    size *= perspective_ratio;

    float fontScale = size / 24.0;

    highp float symbol_rotation = 0.0;
    if (u_rotate_symbol) {
        // Point labels with 'rotation-alignment: map' are horizontal with respect to tile units
        // To figure out that angle in projected space, we draw a short horizontal line in tile
        // space, project it, and measure its angle in projected space.
        vec4 offsetProjectedPoint = u_matrix * vec4(a_pos + vec2(1, 0), 0, 1);

        vec2 a = projectedPoint.xy / projectedPoint.w;
        vec2 b = offsetProjectedPoint.xy / offsetProjectedPoint.w;

        symbol_rotation = atan((b.y - a.y) / u_aspect_ratio, b.x - a.x);
    }

    highp float angle_sin = sin(segment_angle + symbol_rotation);
    highp float angle_cos = cos(segment_angle + symbol_rotation);
    mat2 rotation_matrix = mat2(angle_cos, -1.0 * angle_sin, angle_sin, angle_cos);

    vec4 projected_pos = u_label_plane_matrix * vec4(a_projected_pos.xy, 0.0, 1.0);
    gl_Position = u_coord_matrix * vec4(projected_pos.xy / projected_pos.w + rotation_matrix * (a_offset / 32.0 * fontScale), 0.0, 1.0);
    float gamma_scale = gl_Position.w;

    vec2 fade_opacity = unpack_opacity(a_fade_opacity);
    float fade_change = fade_opacity[1] > 0.5 ? u_fade_change : -u_fade_change;
    float interpolated_fade_opacity = max(0.0, min(1.0, fade_opacity[0] + fade_change));

    v_data0.xy = a_tex / u_texsize;
    v_data0.zw = a_tex / u_texsize_icon;
    v_data1 = vec4(gamma_scale, size, interpolated_fade_opacity, is_sdf);
}

*/

// Uncompressed source of symbol_text_and_icon.fragment.glsl:
/*
#define SDF_PX 8.0

#define SDF 1.0
#define ICON 0.0

uniform bool u_is_halo;
uniform sampler2D u_texture;
uniform sampler2D u_texture_icon;
uniform highp float u_gamma_scale;
uniform lowp float u_device_pixel_ratio;

varying vec4 v_data0;
varying vec4 v_data1;


#ifndef HAS_UNIFORM_u_fill_color
varying highp vec4 fill_color;
#else
uniform highp vec4 u_fill_color;
#endif


#ifndef HAS_UNIFORM_u_halo_color
varying highp vec4 halo_color;
#else
uniform highp vec4 u_halo_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


#ifndef HAS_UNIFORM_u_halo_width
varying lowp float halo_width;
#else
uniform lowp float u_halo_width;
#endif


#ifndef HAS_UNIFORM_u_halo_blur
varying lowp float halo_blur;
#else
uniform lowp float u_halo_blur;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_fill_color
    highp vec4 fill_color = u_fill_color;
#endif

    
#ifdef HAS_UNIFORM_u_halo_color
    highp vec4 halo_color = u_halo_color;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif

    
#ifdef HAS_UNIFORM_u_halo_width
    lowp float halo_width = u_halo_width;
#endif

    
#ifdef HAS_UNIFORM_u_halo_blur
    lowp float halo_blur = u_halo_blur;
#endif


    float fade_opacity = v_data1[2];

    if (v_data1.w == ICON) {
        vec2 tex_icon = v_data0.zw;
        lowp float alpha = opacity * fade_opacity;
        gl_FragColor = texture2D(u_texture_icon, tex_icon) * alpha;

#ifdef OVERDRAW_INSPECTOR
        gl_FragColor = vec4(1.0);
#endif
        return;
    }

    vec2 tex = v_data0.xy;

    float EDGE_GAMMA = 0.105 / u_device_pixel_ratio;

    float gamma_scale = v_data1.x;
    float size = v_data1.y;

    float fontScale = size / 24.0;

    lowp vec4 color = fill_color;
    highp float gamma = EDGE_GAMMA / (fontScale * u_gamma_scale);
    lowp float buff = (256.0 - 64.0) / 256.0;
    if (u_is_halo) {
        color = halo_color;
        gamma = (halo_blur * 1.19 / SDF_PX + EDGE_GAMMA) / (fontScale * u_gamma_scale);
        buff = (6.0 - halo_width / fontScale) / SDF_PX;
    }

    lowp float dist = texture2D(u_texture, tex).a;
    highp float gamma_scaled = gamma * gamma_scale;
    highp float alpha = smoothstep(buff - gamma_scaled, buff + gamma_scaled, dist);

    gl_FragColor = color * (alpha * opacity * fade_opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
