#ifndef MBGL_GEOMETRY_VAO
#define MBGL_GEOMETRY_VAO

#include <mbgl/shader/shader.hpp>
#include <mbgl/platform/gl.hpp>

#include <stdexcept>

namespace mbgl {

#if GL_ARB_vertex_array_object
class VertexArrayObject {
public:
    template <typename Shader, typename VertexBuffer>
    inline void bind(Shader& shader, VertexBuffer &vertexBuffer, char *offset) {
        bindVertexArrayObject();
        if (bound_shader == 0) {
            vertexBuffer.bind();
            shader.bind(offset);
            storeBinding(shader, vertexBuffer.getID(), 0, offset);
        } else {
            verifyBinding(shader, vertexBuffer.getID(), 0, offset);
        }
    }

    template <typename Shader, typename VertexBuffer, typename ElementsBuffer>
    inline void bind(Shader& shader, VertexBuffer &vertexBuffer, ElementsBuffer &elementsBuffer, char *offset) {
        bindVertexArrayObject();
        if (bound_shader == 0) {
            vertexBuffer.bind();
            elementsBuffer.bind();
            shader.bind(offset);
            storeBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
        } else {
            verifyBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
        }
    }

    ~VertexArrayObject();

private:
    void bindVertexArrayObject();
    void storeBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer, char *offset);
    void verifyBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer, char *offset);

    GLuint vao = 0;

    // For debug reasons, we're storing the bind information so that we can
    // detect errors and report
    GLuint bound_shader = 0;
    const char *bound_shader_name = "";
    GLuint bound_vertex_buffer = 0;
    GLuint bound_elements_buffer = 0;
    char *bound_offset = 0;
};

#else

class VertexArrayObject {
public:
    template <typename Shader, typename Buffers>
    void bind(Shader& shader, Buffers& buffers, char *offset) {
        for (auto &buffer : buffers) {
            buffer.bind();
        }
        shader.bind(offset);
    }
};

#endif

}

#endif
