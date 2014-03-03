#ifndef LLMR_GEOMETRY_VAO
#define LLMR_GEOMETRY_VAO

#include <llmr/platform/gl.hpp>

#include <stdexcept>

namespace llmr {

class VertexArrayObject {
public:
    template <typename Shader, typename VertexBuffer>
    void bind(Shader& shader, VertexBuffer& vertex_buffer, char *offset) {
        if (!vao) {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
        } else {
            // We have been given the correct information.
            glBindVertexArray(vao);
        }

        if (shader_ptr != &shader) {
            if (shader_ptr != nullptr) {
                fprintf(stderr, "shader rebind!");
            }
            vertex_buffer.bind();
            shader.bind(offset);

            shader_ptr = &shader;
            vertex_buffer_ptr = &vertex_buffer;
            elements_buffer_ptr = nullptr;
            offset_ptr = offset;
        } else if (vertex_buffer_ptr != &vertex_buffer) {
            throw std::runtime_error("trying to bind VAO to another vertex buffer");
        } else if (elements_buffer_ptr != nullptr) {
            throw std::runtime_error("trying to bind VAO to another elements buffer");
        } else if (offset_ptr != offset) {
            throw std::runtime_error("trying to bind VAO to another offset");
        }
    }

    template <typename Shader, typename VertexBuffer, typename ElementsBuffer>
    void bind(Shader& shader, VertexBuffer& vertex_buffer, ElementsBuffer& elements_buffer, char *offset) {
        if (!vao) {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
        } else {
            // We have been given the correct information.
            glBindVertexArray(vao);
        }

        if (shader_ptr != &shader) {
            vertex_buffer.bind();
            elements_buffer.bind();
            shader.bind(offset);

            shader_ptr = &shader;
            vertex_buffer_ptr = &vertex_buffer;
            elements_buffer_ptr = &elements_buffer;
            offset_ptr = offset;
        } else if (vertex_buffer_ptr != &vertex_buffer) {
            throw std::runtime_error("trying to bind VAO to another vertex buffer");
        } else if (elements_buffer_ptr != &elements_buffer) {
            throw std::runtime_error("trying to bind VAO to another elements buffer");
        } else if (offset_ptr != offset) {
            throw std::runtime_error("trying to bind VAO to another offset");
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
    void *shader_ptr = nullptr;
    void *vertex_buffer_ptr = nullptr;
    void *elements_buffer_ptr = nullptr;
    char *offset_ptr = 0;
};

}

#endif
