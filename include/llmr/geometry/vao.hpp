#ifndef LLMR_GEOMETRY_VAO
#define LLMR_GEOMETRY_VAO

#include <llmr/platform/gl.hpp>

#include <stdexcept>

namespace llmr {

class VertexArrayObject {
public:
    template <typename Shader, typename VertexBuffer>
    void bind(Shader& shader, VertexBuffer& vertex_buffer, char *vertex_offset) {
        if (!vao) {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            vertex_buffer.bind();
            shader.bind_vertex(vertex_offset);

            program = shader.program;
            buffer = &vertex_buffer;
            offset = vertex_offset;
        } else if (program != shader.program) {
            throw std::runtime_error("trying to bind VAO to different program");
        } else if (buffer != &vertex_buffer) {
            throw std::runtime_error("trying to bind VAO to another buffer");
        } else if (offset != vertex_offset) {
            throw std::runtime_error("trying to bind VAO to another offset");
        } else {
            // This is the correct program for this VAO.
            glBindVertexArray(vao);
        }
    }

    ~VertexArrayObject() {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
        }
    }

private:
    GLuint vao = 0;

    // For debug reasons, we're storing the bind information so that we can
    // detect errors and report
    GLuint program = 0;
    void *buffer = NULL;
    char *offset = 0;
};

}

#endif
