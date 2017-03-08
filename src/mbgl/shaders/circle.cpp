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
attribute lowp vec4 a_color_min;
attribute lowp vec4 a_color_max;
varying lowp vec4 color;
uniform lowp float a_radius_t;
attribute mediump float a_radius_min;
attribute mediump float a_radius_max;
varying mediump float radius;
uniform lowp float a_blur_t;
attribute lowp float a_blur_min;
attribute lowp float a_blur_max;
varying lowp float blur;
uniform lowp float a_opacity_t;
attribute lowp float a_opacity_min;
attribute lowp float a_opacity_max;
varying lowp float opacity;
uniform lowp float a_stroke_color_t;
attribute lowp vec4 a_stroke_color_min;
attribute lowp vec4 a_stroke_color_max;
varying lowp vec4 stroke_color;
uniform lowp float a_stroke_width_t;
attribute mediump float a_stroke_width_min;
attribute mediump float a_stroke_width_max;
varying mediump float stroke_width;
uniform lowp float a_stroke_opacity_t;
attribute lowp float a_stroke_opacity_min;
attribute lowp float a_stroke_opacity_max;
varying lowp float stroke_opacity;

varying vec2 v_extrude;
varying lowp float v_antialiasblur;

void main(void) {
    color = mix(a_color_min, a_color_max, a_color_t);
    radius = mix(a_radius_min, a_radius_max, a_radius_t);
    blur = mix(a_blur_min, a_blur_max, a_blur_t);
    opacity = mix(a_opacity_min, a_opacity_max, a_opacity_t);
    stroke_color = mix(a_stroke_color_min, a_stroke_color_max, a_stroke_color_t);
    stroke_width = mix(a_stroke_width_min, a_stroke_width_max, a_stroke_width_t);
    stroke_opacity = mix(a_stroke_opacity_min, a_stroke_opacity_max, a_stroke_opacity_t);

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
varying lowp vec4 color;
varying mediump float radius;
varying lowp float blur;
varying lowp float opacity;
varying lowp vec4 stroke_color;
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
