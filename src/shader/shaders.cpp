// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#include <llmr/shader/shaders.hpp>

using namespace llmr;

const shader_source llmr::shaders[SHADER_COUNT] = {
   [FILL_SHADER] = {
       .vertex = "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n}\n",
       .fragment = "uniform vec4 u_color;\n\nvoid main() {\n    gl_FragColor = u_color;\n}\n",
   },
   [LINE_SHADER] = {
       .vertex = "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\n\nvoid main() {\n    float z = step(32767.0, a_pos.x) * 2.0;\n    gl_Position = u_matrix * vec4(a_pos, z, 1);\n }\n",
       .fragment = "uniform vec4 u_color;\n\nvoid main() {\n    gl_FragColor = u_color;\n}\n",
   },
   [OUTLINE_SHADER] = {
       .vertex = "attribute vec2 a_pos;\nuniform mat4 u_matrix;\nuniform vec2 u_world;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    // If the x coordinate is the maximum integer, we move the z coordinates out\n    // of the view plane so that the triangle gets clipped. This makes it easier\n    // for us to create degenerate triangle strips.\n    float z = step(32767.0, a_pos.x) * 2.0;\n    gl_Position = u_matrix * vec4(a_pos, z, 1);\n\n    v_pos = (gl_Position.xy + 1.0) / 2.0 * u_world;\n}\n",
       .fragment = "uniform vec4 u_color;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    float dist = length(v_pos - gl_FragCoord.xy);\n    float alpha = smoothstep(1.0, 0.0, dist);\n    gl_FragColor = u_color * alpha;\n}\n",
   }
};
