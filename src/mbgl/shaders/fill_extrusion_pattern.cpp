// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/fill_extrusion_pattern.hpp>

namespace mbgl {
namespace shaders {

const char* fill_extrusion_pattern::name = "fill_extrusion_pattern";
const char* fill_extrusion_pattern::vertexSource = R"MBGL_SHADER(
uniform mat4 u_matrix;
uniform vec2 u_pattern_size_a;
uniform vec2 u_pattern_size_b;
uniform vec2 u_pixel_coord_upper;
uniform vec2 u_pixel_coord_lower;
uniform float u_scale_a;
uniform float u_scale_b;
uniform float u_tile_units_to_pixels;
uniform float u_height_factor;

uniform vec3 u_lightcolor;
uniform lowp vec3 u_lightpos;
uniform lowp float u_lightintensity;

attribute vec2 a_pos;
attribute vec4 a_normal_ed;

varying vec2 v_pos_a;
varying vec2 v_pos_b;
varying vec4 v_lighting;
varying float v_directional;


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


    vec3 normal = a_normal_ed.xyz;
    float edgedistance = a_normal_ed.w;

    base = max(0.0, base);
    height = max(0.0, height);

    float t = mod(normal.x, 2.0);
    float z = t > 0.0 ? height : base;

    gl_Position = u_matrix * vec4(a_pos, z, 1);

    vec2 pos = normal.x == 1.0 && normal.y == 0.0 && normal.z == 16384.0
        ? a_pos // extrusion top
        : vec2(edgedistance, z * u_height_factor); // extrusion side

    v_pos_a = get_pattern_pos(u_pixel_coord_upper, u_pixel_coord_lower, u_scale_a * u_pattern_size_a, u_tile_units_to_pixels, pos);
    v_pos_b = get_pattern_pos(u_pixel_coord_upper, u_pixel_coord_lower, u_scale_b * u_pattern_size_b, u_tile_units_to_pixels, pos);

    v_lighting = vec4(0.0, 0.0, 0.0, 1.0);
    float directional = clamp(dot(normal / 16383.0, u_lightpos), 0.0, 1.0);
    directional = mix((1.0 - u_lightintensity), max((0.5 + u_lightintensity), 1.0), directional);

    if (normal.y != 0.0) {
        directional *= clamp((t + base) * pow(height / 150.0, 0.5), mix(0.7, 0.98, 1.0 - u_lightintensity), 1.0);
    }

    v_lighting.rgb += clamp(directional * u_lightcolor, mix(vec3(0.0), vec3(0.3), 1.0 - u_lightcolor), vec3(1.0));
}

)MBGL_SHADER";
const char* fill_extrusion_pattern::fragmentSource = R"MBGL_SHADER(
uniform vec2 u_pattern_tl_a;
uniform vec2 u_pattern_br_a;
uniform vec2 u_pattern_tl_b;
uniform vec2 u_pattern_br_b;
uniform vec2 u_texsize;
uniform float u_mix;

uniform sampler2D u_image;

varying vec2 v_pos_a;
varying vec2 v_pos_b;
varying vec4 v_lighting;


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


void main() {
    
#ifdef HAS_UNIFORM_u_base
    lowp float base = u_base;
#endif

    
#ifdef HAS_UNIFORM_u_height
    lowp float height = u_height;
#endif


    vec2 imagecoord = mod(v_pos_a, 1.0);
    vec2 pos = mix(u_pattern_tl_a / u_texsize, u_pattern_br_a / u_texsize, imagecoord);
    vec4 color1 = texture2D(u_image, pos);

    vec2 imagecoord_b = mod(v_pos_b, 1.0);
    vec2 pos2 = mix(u_pattern_tl_b / u_texsize, u_pattern_br_b / u_texsize, imagecoord_b);
    vec4 color2 = texture2D(u_image, pos2);

    vec4 mixedColor = mix(color1, color2, u_mix);

    gl_FragColor = mixedColor * v_lighting;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
