// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/circle.hpp>

namespace mbgl {
namespace shaders {

const char* circle::name = "circle";
const char* circle::vertexSource = R"MBGL_SHADER(
uniform mat4 u_matrix;
uniform bool u_scale_with_map;
uniform vec2 u_extrude_scale;

attribute vec2 a_pos;

uniform lowp float a_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
uniform lowp float a_radius_t;
attribute mediump vec2 a_radius;
varying mediump float radius;
uniform lowp float a_blur_t;
attribute lowp vec2 a_blur;
varying lowp float blur;
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
uniform lowp float a_stroke_color_t;
attribute highp vec4 a_stroke_color;
varying highp vec4 stroke_color;
uniform lowp float a_stroke_width_t;
attribute mediump vec2 a_stroke_width;
varying mediump float stroke_width;
uniform lowp float a_stroke_opacity_t;
attribute lowp vec2 a_stroke_opacity;
varying lowp float stroke_opacity;

varying vec2 v_extrude;
varying lowp float v_antialiasblur;

void main(void) {
    color = unpack_mix_vec4(a_color, a_color_t);
    radius = unpack_mix_vec2(a_radius, a_radius_t);
    blur = unpack_mix_vec2(a_blur, a_blur_t);
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);
    stroke_color = unpack_mix_vec4(a_stroke_color, a_stroke_color_t);
    stroke_width = unpack_mix_vec2(a_stroke_width, a_stroke_width_t);
    stroke_opacity = unpack_mix_vec2(a_stroke_opacity, a_stroke_opacity_t);

    // unencode the extrusion vector that we snuck into the a_pos vector
    v_extrude = vec2(mod(a_pos, 2.0) * 2.0 - 1.0);

    vec2 extrude = v_extrude * (radius + stroke_width) * u_extrude_scale;
    // multiply a_pos by 0.5, since we had it * 2 in order to sneak
    // in extrusion data
    gl_Position = u_matrix * vec4(floor(a_pos * 0.5), 0, 1);

    if (u_scale_with_map) {
        gl_Position.xy += extrude;
    } else {
        gl_Position.xy += extrude * gl_Position.w;
    }

    // This is a minimum blur distance that serves as a faux-antialiasing for
    // the circle. since blur is a ratio of the circle's size and the intent is
    // to keep the blur at roughly 1px, the two are inversely related.
    v_antialiasblur = 1.0 / DEVICE_PIXEL_RATIO / (radius + stroke_width);
}

)MBGL_SHADER";
const char* circle::fragmentSource = R"MBGL_SHADER(
varying highp vec4 color;
varying mediump float radius;
varying lowp float blur;
varying lowp float opacity;
varying highp vec4 stroke_color;
varying mediump float stroke_width;
varying lowp float stroke_opacity;

varying vec2 v_extrude;
varying lowp float v_antialiasblur;

void main() {
    
    
    
    
    
    
    

    float extrude_length = length(v_extrude);
    float antialiased_blur = -max(blur, v_antialiasblur);

    float opacity_t = smoothstep(0.0, antialiased_blur, extrude_length - 1.0);

    float color_t = stroke_width < 0.01 ? 0.0 : smoothstep(
        antialiased_blur,
        0.0,
        extrude_length - radius / (radius + stroke_width)
    );

    gl_FragColor = opacity_t * mix(color * opacity, stroke_color * stroke_opacity, color_t);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
