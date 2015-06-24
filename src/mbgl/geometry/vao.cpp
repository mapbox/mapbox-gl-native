#include <mbgl/geometry/vao.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/gl_object_store.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread_context.hpp>

namespace mbgl {

static gl::ExtensionFunction<
    void (GLuint array)>
    BindVertexArray({
        {"GL_ARB_vertex_array_object", "glBindVertexArray"},
        {"GL_OES_vertex_array_object", "glBindVertexArrayOES"},
        {"GL_APPLE_vertex_array_object", "glBindVertexArrayAPPLE"}
    });

static gl::ExtensionFunction<
    void (GLsizei n,
          const GLuint* arrays)>
    DeleteVertexArrays({
        {"GL_ARB_vertex_array_object", "glDeleteVertexArrays"},
        {"GL_OES_vertex_array_object", "glDeleteVertexArraysOES"},
        {"GL_APPLE_vertex_array_object", "glDeleteVertexArraysAPPLE"}
    });

static gl::ExtensionFunction<
    void (GLsizei n,
          GLuint* arrays)>
    GenVertexArrays({
        {"GL_ARB_vertex_array_object", "glGenVertexArrays"},
        {"GL_OES_vertex_array_object", "glGenVertexArraysOES"},
        {"GL_APPLE_vertex_array_object", "glGenVertexArraysAPPLE"}
    });

void VertexArrayObject::Unbind() {
    if (!BindVertexArray) return;
    MBGL_CHECK_ERROR(BindVertexArray(0));
}

void VertexArrayObject::Delete(GLsizei n, const GLuint* arrays) {
    MBGL_CHECK_ERROR(DeleteVertexArrays(n, arrays));
}

VertexArrayObject::VertexArrayObject() {
}

VertexArrayObject::~VertexArrayObject() {
    if (!DeleteVertexArrays) return;

    if (vao) {
        util::ThreadContext::getGLObjectStore()->abandonVAO(vao);
    }
}

void VertexArrayObject::bindVertexArrayObject() {
    if (!GenVertexArrays || !BindVertexArray) {
        static bool reported = false;
        if (!reported) {
            Log::Warning(Event::OpenGL, "Not using Vertex Array Objects");
            reported = true;
        }
        return;
    }

    if (!vao) {
        MBGL_CHECK_ERROR(GenVertexArrays(1, &vao));
    }
    MBGL_CHECK_ERROR(BindVertexArray(vao));
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
