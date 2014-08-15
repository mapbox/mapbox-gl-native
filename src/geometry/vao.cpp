#include <mbgl/geometry/vao.hpp>

namespace mbgl {

#if GL_ARB_vertex_array_object

VertexArrayObject::~VertexArrayObject() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
}

void VertexArrayObject::bindVertexArrayObject() {
    if (!vao) {
        glGenVertexArrays(1, &vao);
    }
    glBindVertexArray(vao);
}

void VertexArrayObject::verifyBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer,
                                      char *offset) {
    if (bound_shader != shader.getID()) {
        throw std::runtime_error(std::string("trying to rebind VAO to another shader from " +
                                             std::to_string(bound_shader) + "(" + bound_shader_name + ") to " +
                                             std::to_string(shader.getID()) + "(" + shader.name + ")" ));
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

#endif

}
