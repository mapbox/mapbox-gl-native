#include <mbgl/geometry/vao.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/map/environment.hpp>

namespace mbgl {

VertexArrayObject::VertexArrayObject() {
}

VertexArrayObject::~VertexArrayObject() {
    if (!gl::DeleteVertexArrays) return;

    if (vao) {
        Environment::Get().abandonVAO(vao);
    }
}

void VertexArrayObject::bindVertexArrayObject() {
    if (!gl::GenVertexArrays || !gl::BindVertexArray) {
        static bool reported = false;
        if (!reported) {
            Log::Warning(Event::OpenGL, "Not using Vertex Array Objects");
            reported = true;
        }
        return;
    }

    if (!vao) {
        MBGL_CHECK_ERROR(gl::GenVertexArrays(1, &vao));
    }
    MBGL_CHECK_ERROR(gl::BindVertexArray(vao));
}

void VertexArrayObject::verifyBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer,
                                      char *offset) {
    if (bound_shader != shader.getID()) {
        throw std::runtime_error(std::string("trying to rebind VAO to another shader from " +
                                             util::toString(bound_shader) + "(" + bound_shader_name + ") to " +
                                             util::toString(shader.getID()) + "(" + shader.name + ")" ));
    } else if (bound_offset != offset) {
        throw std::runtime_error("trying to bind VAO to another offset");
    } else if (bound_vertex_buffer != vertexBuffer) {
        throw std::runtime_error("trying to bind VAO to another vertex buffer");
    } else if (bound_elements_buffer != elementsBuffer) {
        throw std::runtime_error("trying to bind VAO to another elements buffer");
    }
}

void VertexArrayObject::storeBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer,
                                     char *offset) {
    bound_shader = shader.getID();
    bound_shader_name = shader.name;
    bound_offset = offset;
    bound_vertex_buffer = vertexBuffer;
    bound_elements_buffer = elementsBuffer;
}

}
