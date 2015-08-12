#include <mbgl/shader/box_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/renderer/gl_config.hpp>

#include <cstdio>

using namespace mbgl;

CollisionBoxShader::CollisionBoxShader()
    : Shader(
        "collisionbox",
        shaders[BOX_SHADER].vertex,
        shaders[BOX_SHADER].fragment,
        [] (GLuint p) {
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 0, "a_pos"));
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 1, "a_extrude"));
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 2, "a_data"));
        }
    ) {
    a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
    a_extrude = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_extrude"));
    a_data = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_data"));
}

void CollisionBoxShader::bind(gl::Config& config, char *offset) {
    config.enableVertexAttributes({ a_pos, a_extrude, a_data });

    const int stride = 12;
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, stride, offset + 0));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_extrude, 2, GL_SHORT, false, stride, offset + 4));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data, 2, GL_UNSIGNED_BYTE, false, stride, offset + 8));

}
