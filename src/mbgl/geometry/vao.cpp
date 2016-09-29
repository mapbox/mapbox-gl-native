#include <mbgl/geometry/vao.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {

VertexArrayObject::VertexArrayObject() {
}

VertexArrayObject::~VertexArrayObject() = default;

void VertexArrayObject::bindVertexArrayObject(gl::Context& context) {
    if (!gl::GenVertexArrays || !gl::BindVertexArray) {
        static bool reported = false;
        if (!reported) {
            Log::Warning(Event::OpenGL, "Not using Vertex Array Objects");
            reported = true;
        }
        return;
    }

    if (!vertexArray) {
        vertexArray = context.createVertexArray();
        context.vertexBuffer.setDirty();
        context.elementBuffer.setDirty();
    }

    context.vertexArrayObject = *vertexArray;
}

void VertexArrayObject::verifyBinding(Shader& shader, GLuint vertexBuffer, GLuint elementsBuffer,
                                      GLbyte *offset) {
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
                                     GLbyte *offset) {
    bound_shader = shader.getID();
    bound_shader_name = shader.name;
    bound_offset = offset;
    bound_vertex_buffer = vertexBuffer;
    bound_elements_buffer = elementsBuffer;
}

} // namespace mbgl
