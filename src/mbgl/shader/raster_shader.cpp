#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/renderer/gl_config.hpp>

#include <cstdio>

using namespace mbgl;

RasterShader::RasterShader()
    : Shader(
        "raster",
        shaders[RASTER_SHADER].vertex,
        shaders[RASTER_SHADER].fragment,
        [] (GLuint p) {
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 0, "a_pos"));
        }
    ) {
    a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
}

void RasterShader::bind(gl::Config& config, char *offset) {
    config.enableVertexAttributes({ a_pos });

    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset));
}
