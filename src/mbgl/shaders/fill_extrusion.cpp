// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/fill_extrusion.hpp>

namespace mbgl {
namespace shaders {

const char* fill_extrusion::name = "fill_extrusion";
const char* fill_extrusion::vertexSource = R"MBGL_SHADER(
uniform mat4 u_matrix;
uniform vec3 u_lightcolor;
uniform lowp vec3 u_lightpos;
uniform lowp float u_lightintensity;

attribute vec2 a_pos;
attribute vec4 a_normal_ed;

varying vec4 v_color;


#ifndef HAS_UNIFORM_u_base
uniform lowp float a_base_t;
attribute lowp vec2 a_base;
varying lowp float base;
#else
uniform lowp float u_base;
#endif


#ifndef HAS_UNIFORM_u_height
uniform lowp float a_height_t;
attribute lowp vec2 a_height;
varying lowp float height;
#else
uniform lowp float u_height;
#endif



#ifndef HAS_UNIFORM_u_color
uniform lowp float a_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_base
    base = unpack_mix_vec2(a_base, a_base_t);
#else
    lowp float base = u_base;
#endif

    
#ifndef HAS_UNIFORM_u_height
    height = unpack_mix_vec2(a_height, a_height_t);
#else
    lowp float height = u_height;
#endif

    
#ifndef HAS_UNIFORM_u_color
    color = unpack_mix_vec4(a_color, a_color_t);
#else
    highp vec4 color = u_color;
#endif


    vec3 normal = a_normal_ed.xyz;

    base = max(0.0, base);
    height = max(0.0, height);

    float t = mod(normal.x, 2.0);

    gl_Position = u_matrix * vec4(a_pos, t > 0.0 ? height : base, 1);

    // Relative luminance (how dark/bright is the surface color?)
    float colorvalue = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;

    v_color = vec4(0.0, 0.0, 0.0, 1.0);

    // Add slight ambient lighting so no extrusions are totally black
    vec4 ambientlight = vec4(0.03, 0.03, 0.03, 1.0);
    color += ambientlight;

    // Calculate cos(theta), where theta is the angle between surface normal and diffuse light ray
    float directional = clamp(dot(normal / 16384.0, u_lightpos), 0.0, 1.0);

    // Adjust directional so that
    // the range of values for highlight/shading is narrower
    // with lower light intensity
    // and with lighter/brighter surface colors
    directional = mix((1.0 - u_lightintensity), max((1.0 - colorvalue + u_lightintensity), 1.0), directional);

    // Add gradient along z axis of side surfaces
    if (normal.y != 0.0) {
        directional *= clamp((t + base) * pow(height / 150.0, 0.5), mix(0.7, 0.98, 1.0 - u_lightintensity), 1.0);
    }

    // Assign final color based on surface + ambient light color, diffuse light directional, and light color
    // with lower bounds adjusted to hue of light
    // so that shading is tinted with the complementary (opposite) color to the light color
    v_color.r += clamp(color.r * directional * u_lightcolor.r, mix(0.0, 0.3, 1.0 - u_lightcolor.r), 1.0);
    v_color.g += clamp(color.g * directional * u_lightcolor.g, mix(0.0, 0.3, 1.0 - u_lightcolor.g), 1.0);
    v_color.b += clamp(color.b * directional * u_lightcolor.b, mix(0.0, 0.3, 1.0 - u_lightcolor.b), 1.0);
}

)MBGL_SHADER";
const char* fill_extrusion::fragmentSource = R"MBGL_SHADER(
varying vec4 v_color;

#ifndef HAS_UNIFORM_u_base
varying lowp float base;
#else
uniform lowp float u_base;
#endif


#ifndef HAS_UNIFORM_u_height
varying lowp float height;
#else
uniform lowp float u_height;
#endif


#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_base
    lowp float base = u_base;
#endif

    
#ifdef HAS_UNIFORM_u_height
    lowp float height = u_height;
#endif

    
#ifdef HAS_UNIFORM_u_color
    highp vec4 color = u_color;
#endif


    gl_FragColor = v_color;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
