// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/line_sdf.hpp>

namespace mbgl {
namespace shaders {

const char* line_sdf::name = "line_sdf";
const char* line_sdf::vertexSource = R"MBGL_SHADER(
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

// the distance over which the line edge fades out.
// Retina devices need a smaller distance to avoid aliasing.
#define ANTIALIASING 1.0 / DEVICE_PIXEL_RATIO / 2.0

attribute vec3 a_pos_normal;
attribute vec4 a_data;

uniform mat4 u_matrix;
uniform mediump float u_ratio;
uniform vec2 u_patternscale_a;
uniform float u_tex_y_a;
uniform vec2 u_patternscale_b;
uniform float u_tex_y_b;
uniform vec2 u_gl_units_to_pixels;

varying vec2 v_normal;
varying vec2 v_width2;
varying vec2 v_tex_a;
varying vec2 v_tex_b;
varying float v_gamma_scale;


#ifndef HAS_UNIFORM_u_color
uniform lowp float a_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_blur
uniform lowp float a_blur_t;
attribute lowp vec2 a_blur;
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


#ifndef HAS_UNIFORM_u_gapwidth
uniform lowp float a_gapwidth_t;
attribute mediump vec2 a_gapwidth;
#else
uniform mediump float u_gapwidth;
#endif


#ifndef HAS_UNIFORM_u_offset
uniform lowp float a_offset_t;
attribute lowp vec2 a_offset;
#else
uniform lowp float u_offset;
#endif


#ifndef HAS_UNIFORM_u_width
uniform lowp float a_width_t;
attribute mediump vec2 a_width;
varying mediump float width;
#else
uniform mediump float u_width;
#endif


#ifndef HAS_UNIFORM_u_floorwidth
uniform lowp float a_floorwidth_t;
attribute lowp vec2 a_floorwidth;
varying lowp float floorwidth;
#else
uniform lowp float u_floorwidth;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_color
    color = unpack_mix_vec4(a_color, a_color_t);
#else
    highp vec4 color = u_color;
#endif

    
#ifndef HAS_UNIFORM_u_blur
    blur = unpack_mix_vec2(a_blur, a_blur_t);
#else
    lowp float blur = u_blur;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif

    
#ifndef HAS_UNIFORM_u_gapwidth
    mediump float gapwidth = unpack_mix_vec2(a_gapwidth, a_gapwidth_t);
#else
    mediump float gapwidth = u_gapwidth;
#endif

    
#ifndef HAS_UNIFORM_u_offset
    lowp float offset = unpack_mix_vec2(a_offset, a_offset_t);
#else
    lowp float offset = u_offset;
#endif

    
#ifndef HAS_UNIFORM_u_width
    width = unpack_mix_vec2(a_width, a_width_t);
#else
    mediump float width = u_width;
#endif

    
#ifndef HAS_UNIFORM_u_floorwidth
    floorwidth = unpack_mix_vec2(a_floorwidth, a_floorwidth_t);
#else
    lowp float floorwidth = u_floorwidth;
#endif


    vec2 a_extrude = a_data.xy - 128.0;
    float a_direction = mod(a_data.z, 4.0) - 1.0;
    float a_linesofar = (floor(a_data.z / 4.0) + a_data.w * 64.0) * LINE_DISTANCE_SCALE;

    vec2 pos = a_pos_normal.xy;

    // transform y normal so that 0 => -1 and 1 => 1
    // In the texture normal, x is 0 if the normal points straight up/down and 1 if it's a round cap
    // y is 1 if the normal points up, and -1 if it points down
    mediump vec2 normal = unpack_float(a_pos_normal.z);
    normal.y = sign(normal.y - 0.5);

    v_normal = normal;

    // these transformations used to be applied in the JS and native code bases.
    // moved them into the shader for clarity and simplicity.
    gapwidth = gapwidth / 2.0;
    float halfwidth = width / 2.0;
    offset = -1.0 * offset;

    float inset = gapwidth + (gapwidth > 0.0 ? ANTIALIASING : 0.0);
    float outset = gapwidth + halfwidth * (gapwidth > 0.0 ? 2.0 : 1.0) + ANTIALIASING;

    // Scale the extrusion vector down to a normal and then up by the line width
    // of this vertex.
    mediump vec2 dist =outset * a_extrude * scale;

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
    float extrude_length_with_perspective = length(projected_extrude.xy / gl_Position.w * u_gl_units_to_pixels);
    v_gamma_scale = extrude_length_without_perspective / extrude_length_with_perspective;

    v_tex_a = vec2(a_linesofar * u_patternscale_a.x / floorwidth, normal.y * u_patternscale_a.y + u_tex_y_a);
    v_tex_b = vec2(a_linesofar * u_patternscale_b.x / floorwidth, normal.y * u_patternscale_b.y + u_tex_y_b);

    v_width2 = vec2(outset, inset);
}

)MBGL_SHADER";
const char* line_sdf::fragmentSource = R"MBGL_SHADER(

uniform sampler2D u_image;
uniform float u_sdfgamma;
uniform float u_mix;

varying vec2 v_normal;
varying vec2 v_width2;
varying vec2 v_tex_a;
varying vec2 v_tex_b;
varying float v_gamma_scale;


#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
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


#ifndef HAS_UNIFORM_u_width
varying mediump float width;
#else
uniform mediump float u_width;
#endif


#ifndef HAS_UNIFORM_u_floorwidth
varying lowp float floorwidth;
#else
uniform lowp float u_floorwidth;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_color
    highp vec4 color = u_color;
#endif

    
#ifdef HAS_UNIFORM_u_blur
    lowp float blur = u_blur;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif

    
#ifdef HAS_UNIFORM_u_width
    mediump float width = u_width;
#endif

    
#ifdef HAS_UNIFORM_u_floorwidth
    lowp float floorwidth = u_floorwidth;
#endif


    // Calculate the distance of the pixel from the line in pixels.
    float dist = length(v_normal) * v_width2.s;

    // Calculate the antialiasing fade factor. This is either when fading in
    // the line in case of an offset line (v_width2.t) or when fading out
    // (v_width2.s)
    float blur2 = (blur + 1.0 / DEVICE_PIXEL_RATIO) * v_gamma_scale;
    float alpha = clamp(min(dist - (v_width2.t - blur2), v_width2.s - dist) / blur2, 0.0, 1.0);

    float sdfdist_a = texture2D(u_image, v_tex_a).a;
    float sdfdist_b = texture2D(u_image, v_tex_b).a;
    float sdfdist = mix(sdfdist_a, sdfdist_b, u_mix);
    alpha *= smoothstep(0.5 - u_sdfgamma / floorwidth, 0.5 + u_sdfgamma / floorwidth, sdfdist);

    gl_FragColor = color * (alpha * opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
