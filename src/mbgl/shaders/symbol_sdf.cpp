// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/symbol_sdf.hpp>

namespace mbgl {
namespace shaders {

const char* symbol_sdf::name = "symbol_sdf";
const char* symbol_sdf::vertexSource = R"MBGL_SHADER(
const float PI = 3.141592653589793;

attribute vec4 a_pos_offset;
attribute vec4 a_data;

// contents of a_size vary based on the type of property value
// used for {text,icon}-size.
// For constants, a_size is disabled.
// For source functions, we bind only one value per vertex: the value of {text,icon}-size evaluated for the current feature.
// For composite functions:
// [ text-size(lowerZoomStop, feature),
//   text-size(upperZoomStop, feature),
//   layoutSize == text-size(layoutZoomLevel, feature) ]
attribute vec3 a_size;
uniform bool u_is_size_zoom_constant;
uniform bool u_is_size_feature_constant;
uniform mediump float u_size_t; // used to interpolate between zoom stops when size is a composite function
uniform mediump float u_size; // used when size is both zoom and feature constant
uniform mediump float u_layout_size; // used when size is feature constant


#ifndef HAS_UNIFORM_u_fill_color
uniform lowp float a_fill_color_t;
attribute highp vec4 a_fill_color;
varying highp vec4 fill_color;
#else
uniform highp vec4 u_fill_color;
#endif

#ifndef HAS_UNIFORM_u_halo_color
uniform lowp float a_halo_color_t;
attribute highp vec4 a_halo_color;
varying highp vec4 halo_color;
#else
uniform highp vec4 u_halo_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif

#ifndef HAS_UNIFORM_u_halo_width
uniform lowp float a_halo_width_t;
attribute lowp vec2 a_halo_width;
varying lowp float halo_width;
#else
uniform lowp float u_halo_width;
#endif

#ifndef HAS_UNIFORM_u_halo_blur
uniform lowp float a_halo_blur_t;
attribute lowp vec2 a_halo_blur;
varying lowp float halo_blur;
#else
uniform lowp float u_halo_blur;
#endif

// matrix is for the vertex position.
uniform mat4 u_matrix;

uniform bool u_is_text;
uniform mediump float u_zoom;
uniform bool u_rotate_with_map;
uniform bool u_pitch_with_map;
uniform mediump float u_pitch;
uniform mediump float u_bearing;
uniform mediump float u_aspect_ratio;
uniform vec2 u_extrude_scale;

uniform vec2 u_texsize;

varying vec4 v_data0;
varying vec2 v_data1;

void main() {

#ifndef HAS_UNIFORM_u_fill_color
    fill_color = unpack_mix_vec4(a_fill_color, a_fill_color_t);
#else
    highp vec4 fill_color = u_fill_color;
#endif

#ifndef HAS_UNIFORM_u_halo_color
    halo_color = unpack_mix_vec4(a_halo_color, a_halo_color_t);
#else
    highp vec4 halo_color = u_halo_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif

#ifndef HAS_UNIFORM_u_halo_width
    halo_width = unpack_mix_vec2(a_halo_width, a_halo_width_t);
#else
    lowp float halo_width = u_halo_width;
#endif

#ifndef HAS_UNIFORM_u_halo_blur
    halo_blur = unpack_mix_vec2(a_halo_blur, a_halo_blur_t);
#else
    lowp float halo_blur = u_halo_blur;
#endif

    vec2 a_pos = a_pos_offset.xy;
    vec2 a_offset = a_pos_offset.zw;

    vec2 a_tex = a_data.xy;

    mediump vec2 label_data = unpack_float(a_data[2]);
    mediump float a_labelminzoom = label_data[0];
    mediump float a_labelangle = label_data[1];

    mediump vec2 a_zoom = unpack_float(a_data[3]);
    mediump float a_minzoom = a_zoom[0];
    mediump float a_maxzoom = a_zoom[1];
    float size;

    // In order to accommodate placing labels around corners in
    // symbol-placement: line, each glyph in a label could have multiple
    // "quad"s only one of which should be shown at a given zoom level.
    // The min/max zoom assigned to each quad is based on the font size at
    // the vector tile's zoom level, which might be different than at the
    // currently rendered zoom level if text-size is zoom-dependent.
    // Thus, we compensate for this difference by calculating an adjustment
    // based on the scale of rendered text size relative to layout text size.
    mediump float layoutSize;
    if (!u_is_size_zoom_constant && !u_is_size_feature_constant) {
        size = mix(a_size[0], a_size[1], u_size_t) / 10.0;
        layoutSize = a_size[2] / 10.0;
    } else if (u_is_size_zoom_constant && !u_is_size_feature_constant) {
        size = a_size[0] / 10.0;
        layoutSize = size;
    } else if (!u_is_size_zoom_constant && u_is_size_feature_constant) {
        size = u_size;
        layoutSize = u_layout_size;
    } else {
        size = u_size;
        layoutSize = u_size;
    }

    float fontScale = u_is_text ? size / 24.0 : size;

    mediump float zoomAdjust = log2(size / layoutSize);
    mediump float adjustedZoom = (u_zoom - zoomAdjust) * 10.0;
    // result: z = 0 if a_minzoom <= adjustedZoom < a_maxzoom, and 1 otherwise
    // Used below to move the vertex out of the clip space for when the current
    // zoom is out of the glyph's zoom range.
    mediump float z = 2.0 - step(a_minzoom, adjustedZoom) - (1.0 - step(a_maxzoom, adjustedZoom));

    // pitch-alignment: map
    // rotation-alignment: map | viewport
    if (u_pitch_with_map) {
        lowp float angle = u_rotate_with_map ? (a_labelangle / 256.0 * 2.0 * PI) : u_bearing;
        lowp float asin = sin(angle);
        lowp float acos = cos(angle);
        mat2 RotationMatrix = mat2(acos, asin, -1.0 * asin, acos);
        vec2 offset = RotationMatrix * a_offset;
        vec2 extrude = fontScale * u_extrude_scale * (offset / 64.0);
        gl_Position = u_matrix * vec4(a_pos + extrude, 0, 1);
        gl_Position.z += z * gl_Position.w;
    // pitch-alignment: viewport
    // rotation-alignment: map
    } else if (u_rotate_with_map) {
        // foreshortening factor to apply on pitched maps
        // as a label goes from horizontal <=> vertical in angle
        // it goes from 0% foreshortening to up to around 70% foreshortening
        lowp float pitchfactor = 1.0 - cos(u_pitch * sin(u_pitch * 0.75));

        lowp float lineangle = a_labelangle / 256.0 * 2.0 * PI;

        // use the lineangle to position points a,b along the line
        // project the points and calculate the label angle in projected space
        // this calculation allows labels to be rendered unskewed on pitched maps
        vec4 a = u_matrix * vec4(a_pos, 0, 1);
        vec4 b = u_matrix * vec4(a_pos + vec2(cos(lineangle),sin(lineangle)), 0, 1);
        lowp float angle = atan((b[1]/b[3] - a[1]/a[3])/u_aspect_ratio, b[0]/b[3] - a[0]/a[3]);
        lowp float asin = sin(angle);
        lowp float acos = cos(angle);
        mat2 RotationMatrix = mat2(acos, -1.0 * asin, asin, acos);

        vec2 offset = RotationMatrix * (vec2((1.0-pitchfactor)+(pitchfactor*cos(angle*2.0)), 1.0) * a_offset);
        vec2 extrude = fontScale * u_extrude_scale * (offset / 64.0);
        gl_Position = u_matrix * vec4(a_pos, 0, 1) + vec4(extrude, 0, 0);
        gl_Position.z += z * gl_Position.w;
    // pitch-alignment: viewport
    // rotation-alignment: viewport
    } else {
        vec2 extrude = fontScale * u_extrude_scale * (a_offset / 64.0);
        gl_Position = u_matrix * vec4(a_pos, 0, 1) + vec4(extrude, 0, 0);
    }

    float gamma_scale = gl_Position.w;

    vec2 tex = a_tex / u_texsize;
    vec2 fade_tex = vec2(a_labelminzoom / 255.0, 0.0);

    v_data0 = vec4(tex.x, tex.y, fade_tex.x, fade_tex.y);
    v_data1 = vec2(gamma_scale, size);
}

)MBGL_SHADER";
const char* symbol_sdf::fragmentSource = R"MBGL_SHADER(
#define SDF_PX 8.0
#define EDGE_GAMMA 0.105/DEVICE_PIXEL_RATIO

uniform bool u_is_halo;

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

uniform sampler2D u_texture;
uniform sampler2D u_fadetexture;
uniform highp float u_gamma_scale;
uniform bool u_is_text;

varying vec4 v_data0;
varying vec2 v_data1;

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

    vec2 tex = v_data0.xy;
    vec2 fade_tex = v_data0.zw;
    float gamma_scale = v_data1.x;
    float size = v_data1.y;

    float fontScale = u_is_text ? size / 24.0 : size;

    lowp vec4 color = fill_color;
    highp float gamma = EDGE_GAMMA / (fontScale * u_gamma_scale);
    lowp float buff = (256.0 - 64.0) / 256.0;
    if (u_is_halo) {
        color = halo_color;
        gamma = (halo_blur * 1.19 / SDF_PX + EDGE_GAMMA) / (fontScale * u_gamma_scale);
        buff = (6.0 - halo_width / fontScale) / SDF_PX;
    }

    lowp float dist = texture2D(u_texture, tex).a;
    lowp float fade_alpha = texture2D(u_fadetexture, fade_tex).a;
    highp float gamma_scaled = gamma * gamma_scale;
    highp float alpha = smoothstep(buff - gamma_scaled, buff + gamma_scaled, dist) * fade_alpha;

    gl_FragColor = color * (alpha * opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
