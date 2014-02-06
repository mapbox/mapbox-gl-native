#ifndef LLMR_GEOMETRY_VAO
#define LLMR_GEOMETRY_VAO

#include <llmr/platform/gl.hpp>

#include <stdexcept>

namespace llmr {

template <typename Shader>
class VertexArrayObject {
public:
    template <typename Buffer>
    void bind(Shader& shader, Buffer& buffer, char *offset) {
        if (!vao) {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            buffer.bind();
            shader.bind(offset);

            buffer_ptr = &buffer;
            offset_ptr = offset;
        } else if (buffer_ptr != &buffer) {
            throw std::runtime_error("trying to bind VAO to another buffer");
        } else if (offset_ptr != offset) {
            throw std::runtime_error("trying to bind VAO to another offset");
        } else {
            // We have been given the correct information.
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
    void *buffer_ptr = nullptr;
    char *offset_ptr = 0;
};

}

#endif
