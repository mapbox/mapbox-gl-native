// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/fill_outline.hpp>

namespace mbgl {
namespace shaders {

const char* fill_outline::name = "fill_outline";
const char* fill_outline::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;
uniform vec2 u_world;

varying vec2 v_pos;


#ifndef HAS_UNIFORM_u_outline_color
uniform lowp float a_outline_color_t;
attribute highp vec4 a_outline_color;
varying highp vec4 outline_color;
#else
uniform highp vec4 u_outline_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_outline_color
    outline_color = unpack_mix_vec4(a_outline_color, a_outline_color_t);
#else
    highp vec4 outline_color = u_outline_color;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif


    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    v_pos = (gl_Position.xy / gl_Position.w + 1.0) / 2.0 * u_world;
}

)MBGL_SHADER";
const char* fill_outline::fragmentSource = R"MBGL_SHADER(

#ifndef HAS_UNIFORM_u_outline_color
varying highp vec4 outline_color;
#else
uniform highp vec4 u_outline_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


varying vec2 v_pos;

void main() {
    
#ifdef HAS_UNIFORM_u_outline_color
    highp vec4 outline_color = u_outline_color;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif


    float dist = length(v_pos - gl_FragCoord.xy);
    float alpha = 1.0 - smoothstep(0.0, 1.0, dist);
    gl_FragColor = outline_color * (alpha * opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
