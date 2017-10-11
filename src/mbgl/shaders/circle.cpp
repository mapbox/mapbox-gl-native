// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/circle.hpp>

namespace mbgl {
namespace shaders {

const char* circle::name = "circle";
const char* circle::vertexSource = R"MBGL_SHADER(
uniform mat4 u_matrix;
uniform bool u_scale_with_map;
uniform bool u_pitch_with_map;
uniform vec2 u_extrude_scale;
uniform highp float u_camera_to_center_distance;

attribute vec2 a_pos;


#ifndef HAS_UNIFORM_u_color
uniform lowp float a_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_radius
uniform lowp float a_radius_t;
attribute mediump vec2 a_radius;
varying mediump float radius;
#else
uniform mediump float u_radius;
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


#ifndef HAS_UNIFORM_u_stroke_color
uniform lowp float a_stroke_color_t;
attribute highp vec4 a_stroke_color;
varying highp vec4 stroke_color;
#else
uniform highp vec4 u_stroke_color;
#endif


#ifndef HAS_UNIFORM_u_stroke_width
uniform lowp float a_stroke_width_t;
attribute mediump vec2 a_stroke_width;
varying mediump float stroke_width;
#else
uniform mediump float u_stroke_width;
#endif


#ifndef HAS_UNIFORM_u_stroke_opacity
uniform lowp float a_stroke_opacity_t;
attribute lowp vec2 a_stroke_opacity;
varying lowp float stroke_opacity;
#else
uniform lowp float u_stroke_opacity;
#endif


varying vec3 v_data;

void main(void) {
    
#ifndef HAS_UNIFORM_u_color
    color = unpack_mix_vec4(a_color, a_color_t);
#else
    highp vec4 color = u_color;
#endif

    
#ifndef HAS_UNIFORM_u_radius
    radius = unpack_mix_vec2(a_radius, a_radius_t);
#else
    mediump float radius = u_radius;
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

    
#ifndef HAS_UNIFORM_u_stroke_color
    stroke_color = unpack_mix_vec4(a_stroke_color, a_stroke_color_t);
#else
    highp vec4 stroke_color = u_stroke_color;
#endif

    
#ifndef HAS_UNIFORM_u_stroke_width
    stroke_width = unpack_mix_vec2(a_stroke_width, a_stroke_width_t);
#else
    mediump float stroke_width = u_stroke_width;
#endif

    
#ifndef HAS_UNIFORM_u_stroke_opacity
    stroke_opacity = unpack_mix_vec2(a_stroke_opacity, a_stroke_opacity_t);
#else
    lowp float stroke_opacity = u_stroke_opacity;
#endif


    // unencode the extrusion vector that we snuck into the a_pos vector
    vec2 extrude = vec2(mod(a_pos, 2.0) * 2.0 - 1.0);

    // multiply a_pos by 0.5, since we had it * 2 in order to sneak
    // in extrusion data
    vec2 circle_center = floor(a_pos * 0.5);
    if (u_pitch_with_map) {
        vec2 corner_position = circle_center;
        if (u_scale_with_map) {
            corner_position += extrude * (radius + stroke_width) * u_extrude_scale;
        } else {
            // Pitching the circle with the map effectively scales it with the map
            // To counteract the effect for pitch-scale: viewport, we rescale the
            // whole circle based on the pitch scaling effect at its central point
            vec4 projected_center = u_matrix * vec4(circle_center, 0, 1);
            corner_position += extrude * (radius + stroke_width) * u_extrude_scale * (projected_center.w / u_camera_to_center_distance);
        }

        gl_Position = u_matrix * vec4(corner_position, 0, 1);
    } else {
        gl_Position = u_matrix * vec4(circle_center, 0, 1);

        if (u_scale_with_map) {
            gl_Position.xy += extrude * (radius + stroke_width) * u_extrude_scale * u_camera_to_center_distance;
        } else {
            gl_Position.xy += extrude * (radius + stroke_width) * u_extrude_scale * gl_Position.w;
        }
    }

    // This is a minimum blur distance that serves as a faux-antialiasing for
    // the circle. since blur is a ratio of the circle's size and the intent is
    // to keep the blur at roughly 1px, the two are inversely related.
    lowp float antialiasblur = 1.0 / DEVICE_PIXEL_RATIO / (radius + stroke_width);

    v_data = vec3(extrude.x, extrude.y, antialiasblur);
}

)MBGL_SHADER";
const char* circle::fragmentSource = R"MBGL_SHADER(

#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_radius
varying mediump float radius;
#else
uniform mediump float u_radius;
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


#ifndef HAS_UNIFORM_u_stroke_color
varying highp vec4 stroke_color;
#else
uniform highp vec4 u_stroke_color;
#endif


#ifndef HAS_UNIFORM_u_stroke_width
varying mediump float stroke_width;
#else
uniform mediump float u_stroke_width;
#endif


#ifndef HAS_UNIFORM_u_stroke_opacity
varying lowp float stroke_opacity;
#else
uniform lowp float u_stroke_opacity;
#endif


varying vec3 v_data;

void main() {
    
#ifdef HAS_UNIFORM_u_color
    highp vec4 color = u_color;
#endif

    
#ifdef HAS_UNIFORM_u_radius
    mediump float radius = u_radius;
#endif

    
#ifdef HAS_UNIFORM_u_blur
    lowp float blur = u_blur;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif

    
#ifdef HAS_UNIFORM_u_stroke_color
    highp vec4 stroke_color = u_stroke_color;
#endif

    
#ifdef HAS_UNIFORM_u_stroke_width
    mediump float stroke_width = u_stroke_width;
#endif

    
#ifdef HAS_UNIFORM_u_stroke_opacity
    lowp float stroke_opacity = u_stroke_opacity;
#endif


    vec2 extrude = v_data.xy;
    float extrude_length = length(extrude);

    lowp float antialiasblur = v_data.z;
    float antialiased_blur = -max(blur, antialiasblur);

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
