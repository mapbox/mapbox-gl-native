// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/symbol_icon.hpp>

namespace mbgl {
namespace shaders {

const char* symbol_icon::name = "symbol_icon";
const char* symbol_icon::vertexSource = R"MBGL_SHADER(

attribute vec4 a_pos_offset;
attribute vec2 a_texture_pos;
attribute vec4 a_data;

uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;

// matrix is for the vertex position.
uniform mat4 u_matrix;

uniform mediump float u_zoom;
uniform bool u_rotate_with_map;
uniform vec2 u_extrude_scale;

uniform vec2 u_texsize;

varying vec2 v_tex;
varying vec2 v_fade_tex;

void main() {
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);

    vec2 a_pos = a_pos_offset.xy;
    vec2 a_offset = a_pos_offset.zw;

    vec2 a_tex = a_texture_pos.xy;
    mediump float a_labelminzoom = a_data[0];
    mediump vec2 a_zoom = a_data.pq;
    mediump float a_minzoom = a_zoom[0];
    mediump float a_maxzoom = a_zoom[1];

    // u_zoom is the current zoom level adjusted for the change in font size
    mediump float z = 2.0 - step(a_minzoom, u_zoom) - (1.0 - step(a_maxzoom, u_zoom));

    vec2 extrude = u_extrude_scale * (a_offset / 64.0);
    if (u_rotate_with_map) {
        gl_Position = u_matrix * vec4(a_pos + extrude, 0, 1);
        gl_Position.z += z * gl_Position.w;
    } else {
        gl_Position = u_matrix * vec4(a_pos, 0, 1) + vec4(extrude, 0, 0);
    }

    v_tex = a_tex / u_texsize;
    v_fade_tex = vec2(a_labelminzoom / 255.0, 0.0);
}

)MBGL_SHADER";
const char* symbol_icon::fragmentSource = R"MBGL_SHADER(
uniform sampler2D u_texture;
uniform sampler2D u_fadetexture;

varying lowp float opacity;

varying vec2 v_tex;
varying vec2 v_fade_tex;

void main() {
    

    lowp float alpha = texture2D(u_fadetexture, v_fade_tex).a * opacity;
    gl_FragColor = texture2D(u_texture, v_tex) * alpha;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
