// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/fill.hpp>

namespace mbgl {
namespace shaders {

const char* fill::name = "fill";
const char* fill::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;


#ifndef HAS_UNIFORM_u_color
uniform lowp float a_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_color
    color = unpack_mix_vec4(a_color, a_color_t);
#else
    highp vec4 color = u_color;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif


    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

)MBGL_SHADER";
const char* fill::fragmentSource = R"MBGL_SHADER(

#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_color
    highp vec4 color = u_color;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif


    gl_FragColor = color * opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
