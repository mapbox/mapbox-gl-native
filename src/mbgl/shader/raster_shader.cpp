#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/raster.vertex.hpp>
#include <mbgl/shader/raster.fragment.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

RasterShader::RasterShader(gl::ObjectStore& store)
    : Shader(shaders::raster::name, shaders::raster::vertex, shaders::raster::fragment, store) {
}

void RasterShader::bind(GLbyte* offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset));
}

} // namespace mbgl
