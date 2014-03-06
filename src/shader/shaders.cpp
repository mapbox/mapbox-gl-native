// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#include <llmr/shader/shaders.hpp>

using namespace llmr;

const shader_source llmr::shaders[SHADER_COUNT] = {
   {
       "// floor(127 / 2) == 63.0\n// the maximum allowed miter limit is 2.0 at the moment. the extrude normal is\n// stored in a byte (-128..127). we scale regular normals up to length 63, but\n// there are also \"special\" normals that have a bigger length (of up to 126 in\n// this case).\n// #define scale 63.0\n#define scale 0.015873016\n\nattribute vec2 a_pos;\nattribute vec2 a_extrude;\n// attribute float a_linesofar;\n\n// posmatrix is for the vertex position, exmatrix is for rotating and projecting\n// the extrusion vector.\nuniform mat4 u_matrix;\nuniform mat4 u_exmatrix;\n\n// shared\nuniform float u_ratio;\nuniform vec2 u_linewidth;\nuniform vec4 u_color;\n\nvarying vec2 v_normal;\n// varying float v_linesofar;\n\nvoid main() {\n    // We store the texture normals in the most insignificant bit\n    // transform y so that 0 => -1 and 1 => 1\n    // In the texture normal, x is 0 if the normal points straight up/down and 1 if it's a round cap\n    // y is 1 if the normal points up, and -1 if it points down\n    vec2 normal = mod(a_pos, 2.0);\n    normal.y = sign(normal.y - 0.5);\n    v_normal = normal;\n\n    // Scale the extrusion vector down to a normal and then up by the line width\n    // of this vertex.\n    vec4 dist = vec4(u_linewidth.s * a_extrude * scale, 0.0, 0.0);\n\n    // Remove the texture normal bit of the position before scaling it with the\n    // model/view matrix. Add the extrusion vector *after* the model/view matrix\n    // because we're extruding the line in pixel space, regardless of the current\n    // tile's zoom level.\n    gl_Position = u_matrix * vec4(floor(a_pos * 0.5), 0.0, 1.0) + u_exmatrix * dist;\n    // v_linesofar = a_linesofar * u_ratio;\n}",
       "uniform vec2 u_linewidth;\nuniform vec4 u_color;\n\nuniform vec2 u_dasharray;\n\nvarying vec2 v_normal;\n// varying float v_linesofar;\n\nvoid main() {\n    // Calculate the distance of the pixel from the line in pixels.\n    float dist = length(v_normal) * u_linewidth.s;\n\n    // Calculate the antialiasing fade factor. This is either when fading in\n    // the line in case of an offset line (v_linewidth.t) or when fading out\n    // (v_linewidth.s)\n    float alpha = clamp(min(dist - (u_linewidth.t - 1.0), u_linewidth.s - dist), 0.0, 1.0);\n\n    // Calculate the antialiasing fade factor based on distance to the dash.\n    // Only affects alpha when line is dashed\n    // float pos = mod(v_linesofar, u_dasharray.x + u_dasharray.y);\n    // alpha *= max(step(0.0, -u_dasharray.y), clamp(min(pos, u_dasharray.x - pos), 0.0, 1.0));\n\n    gl_FragColor = u_color * alpha;\n}",
   },
   {
       "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\nuniform vec2 u_world;\nuniform float u_size;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(floor(a_pos / 2.0), 0.0, 1.0);\n    v_pos = (gl_Position.xy + 1.0) * u_world;\n    gl_PointSize = u_size;\n}\n",
       "uniform vec4 u_color;\nuniform vec2 u_linewidth;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    float dist = length(v_pos - gl_FragCoord.xy);\n\n    // Calculate the antialiasing fade factor. This is either when fading in\n    // the line in case of an offset line (v_linewidth.t) or when fading out\n    // (v_linewidth.s)\n    float alpha = clamp(min(dist - (u_linewidth.t - 1.0), u_linewidth.s - dist), 0.0, 1.0);\n    gl_FragColor = u_color * alpha;\n}\n",
   },
   {
       "attribute vec2 a_pos;\nuniform mat4 u_matrix;\nuniform vec2 u_world;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n    v_pos = (gl_Position.xy + 1.0) / 2.0 * u_world;\n}\n",
       "uniform vec4 u_color;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    float dist = length(v_pos - gl_FragCoord.xy);\n    float alpha = smoothstep(1.0, 0.0, dist);\n    gl_FragColor = u_color * alpha;\n}\n",
   },
   {
       "uniform mat4 u_matrix;\n\nattribute vec2 a_pos;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    v_pos = a_pos;\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n}\n",
       "uniform vec4 u_color;\n\nuniform vec2 u_offset;\nuniform vec2 u_pattern_size;\nuniform vec2 u_pattern_tl;\nuniform vec2 u_pattern_br;\nuniform float u_mix;\n\n\nuniform sampler2D u_image;\n\nvarying vec2 v_pos;\n\nvoid main() {\n\n    vec2 imagecoord = mod((v_pos + u_offset) / u_pattern_size, 1.0);\n    vec2 pos = mix(u_pattern_tl, u_pattern_br, imagecoord);\n    vec4 color1 = texture2D(u_image, pos);\n\n    vec2 imagecoord2 = mod(imagecoord * 2.0, 1.0);\n    vec2 pos2 = mix(u_pattern_tl, u_pattern_br, imagecoord2);\n    vec4 color2 = texture2D(u_image, pos2);\n\n    vec4 color = mix(color1, color2, u_mix);\n    gl_FragColor = color + u_color * (1.0 - color.a);\n}\n",
   },
   {
       "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n}\n",
       "uniform vec4 u_color;\n\nvoid main() {\n    gl_FragColor = u_color;\n}\n",
   },
   {
       "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\nuniform float u_size;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n    gl_PointSize = u_size;\n}\n",
       "#define root2 1.4142135623730951\n\nuniform sampler2D u_image;\nuniform vec2 u_tl;\nuniform vec2 u_br;\nuniform vec4 u_color;\n\nuniform vec2 pos;\nuniform float inbounds;\nuniform vec4 color;\n\nvoid main() {\n    vec2 pos = (gl_PointCoord * 2.0 - 1.0) * root2 / 2.0 + 0.5;\n\n    float inbounds = step(0.0, pos.x) * step(0.0, pos.y) *\n        (1.0 - step(1.0, pos.x)) * (1.0 - step(1.0, pos.y));\n\n    vec4 color = texture2D(u_image, mix(u_tl, u_br, pos)) * inbounds;\n\n    gl_FragColor = color * u_color;\n}\n",
   }
};
