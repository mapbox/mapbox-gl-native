// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#include <llmr/shader/shaders.h>

const llmr_shader_t llmr_shaders[SHADER_COUNT] = {
   [FILL_SHADER] = {
       .vertex = "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n}\n",
       .fragment = "uniform vec4 u_color;\n\nvoid main() {\n    gl_FragColor = u_color;\n}\n",
   },
   [LINE_SHADER] = {
       .vertex = "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\n\nvoid main() {\n    float z = step(32767.0, a_pos.x) * 2.0;\n    gl_Position = u_matrix * vec4(a_pos, z, 1);\n }\n",
       .fragment = "uniform vec4 u_color;\n\nvoid main() {\n    gl_FragColor = u_color;\n}\n",
   }
};
